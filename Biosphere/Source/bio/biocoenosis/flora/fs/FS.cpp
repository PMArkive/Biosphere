#include <bio/biocoenosis/flora/fs/FS.hpp>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dirent.h>
#include <sys/iosupport.h>
#include <sys/param.h>
#include <unistd.h>

namespace bio::fs
{
    struct inFile
    {
        fsp::File *fd;
        int flags;
        u64 offset;
    };

    struct inDirectory
    {
        u32 magic;
        fsp::Directory *fd;
        ssize_t idx;
        size_t size;
        fsp::DirectoryEntry data[0x20];
    };

    struct inDevice
    {
        bool setup;
        s32 id;
        devoptab_t device;
        fsp::FileSystem *fs;
        char name[0x20];
    };

    static bool fsinit = false;
    static s32 devdefault = -1;
    static s32 devcwd = -1;
    static inDevice devices[0x20];
    static char cwd[4097] = "/";
    static __thread char fixedpath[4097];
    static fsp::FspService *sfsp;

    static int inFS_open(struct _reent *r, void *fileStruct, const char *path, int flags, int mode);
    static int inFS_close(struct _reent *r, void *fd);
    static ssize_t inFS_write(struct _reent *r, void *fd, const char *ptr, size_t len);
    static ssize_t inFS_write_safe(struct _reent *r, void *fd, const char *ptr, size_t len);
    static ssize_t inFS_read(struct _reent *r, void *fd, char *ptr, size_t len);
    static ssize_t inFS_read_safe(struct _reent *r, void *fd, char *ptr, size_t len);
    static off_t inFS_seek(struct _reent *r, void *fd, off_t pos, int dir);
    static int inFS_fstat(struct _reent *r, void *fd, struct stat *st);
    static int inFS_stat(struct _reent *r, const char *file, struct stat *st);
    static int inFS_link(struct _reent *r, const char *existing, const char *newLink);
    static int inFS_unlink(struct _reent *r, const char *name);
    static int inFS_chdir(struct _reent *r, const char *name);
    static int inFS_rename(struct _reent *r, const char *oldName, const char *newName);
    static int inFS_mkdir(struct _reent *r, const char *path, int mode);
    static DIR_ITER* inFS_diropen(struct _reent *r, DIR_ITER *dirState, const char *path);
    static int inFS_dirreset(struct _reent *r, DIR_ITER *dirState);
    static int inFS_dirnext(struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat);
    static int inFS_dirclose(struct _reent *r, DIR_ITER *dirState);
    static int inFS_statvfs(struct _reent *r, const char *path, struct statvfs *buf);
    static int inFS_ftruncate(struct _reent *r, void *fd, off_t len);
    static int inFS_fsync(struct _reent *r, void *fd);
    static int inFS_chmod(struct _reent *r, const char *path, mode_t mode);
    static int inFS_fchmod(struct _reent *r, void *fd, mode_t mode);
    static int inFS_rmdir(struct _reent *r, const char *name);

    static devoptab_t devoptab =
    {
        .structSize = sizeof(inFile),
        .open_r = inFS_open,
        .close_r = inFS_close,
        .write_r = inFS_write,
        .read_r = inFS_read,
        .seek_r = inFS_seek,
        .fstat_r = inFS_fstat,
        .stat_r = inFS_stat,
        .link_r = inFS_link,
        .unlink_r = inFS_unlink,
        .chdir_r = inFS_chdir,
        .rename_r = inFS_rename,
        .mkdir_r = inFS_mkdir,
        .dirStateSize = sizeof(inDirectory),
        .diropen_r = inFS_diropen,
        .dirreset_r = inFS_dirreset,
        .dirnext_r = inFS_dirnext,
        .dirclose_r = inFS_dirclose,
        .statvfs_r = inFS_statvfs,
        .ftruncate_r = inFS_ftruncate,
        .fsync_r = inFS_fsync,
        .deviceData = 0,
        .chmod_r = inFS_chmod,
        .fchmod_r = inFS_fchmod,
        .rmdir_r = inFS_rmdir,
    };

    static inDevice *inFindDevice(const char *name)
    {
        u32 total = (sizeof(devices) / sizeof(inDevice));
        inDevice *dev = NULL;
        if(!fsinit) return NULL;
        if(name && (name[0] == '/'))
        {
            if(devdefault == -1) return NULL;
            dev = &devices[devdefault];
            if(!dev->setup) return NULL;
            return dev;
        }
        for(u32 i = 0; i < total; i++)
        {
            dev = &devices[i];
            if(name == NULL)
            {
                if(!dev->setup) return dev;
            }
            else if(dev->setup)
            {
                if(strncmp(dev->name, name, strlen(dev->name)) == 0) return dev;
            }
        }
        return NULL;
    }

    static const char *inFixPath(struct _reent *r, const char *path, inDevice **dev)
    {
        ssize_t ;
        uint32_t code;
        const uint8_t *p = (const uint8_t*)path;
        const char *device_path = path;
        do
        {
            units = utf::DecodeUtf8(&code, p);
            if(units < 0)
            {
                r->_errno = EILSEQ;
                return NULL;
            }
            p += units;
        } while(code != ':' && code != 0);
        if(code == ':') path = (const char*)p;
        p = (const uint8_t*)path;
        do
        {
            units = utf::DecodeUtf8(&code, p);
            if(units < 0)
            {
                r->_errno = EILSEQ;
                return NULL;
            }
            if(code == ':')
            {
                r->_errno = EINVAL;
                return NULL;
            }
            p += units;
        } while(code != 0);
        if(path[0] == '/') strncpy(fixedpath, path, PATH_MAX);
        else
        {
            strncpy(fixedpath, __cwd, PATH_MAX);
            fixedpath[PATH_MAX] = '\0';
            strncat(fixedpath, path, PATH_MAX - strlen(__cwd));
        }
        if(fixedpath[PATH_MAX] != 0)
        {
            fixedpath[PATH_MAX] = 0;
            r->_errno = ENAMETOOLONG;
            return NULL;
        }

        if(dev)
        {
            if(path[0] == '/') *dev = inFindDevice(device_path);
            else
            {
                *dev = NULL;
                if(devcwd != -1) *dev = &devices[cwd];
            }
            if(*dev == NULL)
            {
                r->_errno = ENODEV;
                return NULL;
            }
        }
        return fixedpath;
    }

    static int inGetFSPath(struct _reent *r, const char *path, inDevice **dev, char *out)
    {
        if(inFixPath(r, path, dev) == NULL) return -1;
        memcpy(out, fixedpath, 0x302);
        out[0x301] = '\0';
        return 0;
    }

    static ssize_t inConvertFromFSPath(u8 *out, u8 *in, size_t len)
    {
        strncpy((char*)out, (char*)in, len);
        return strnlen((char*)out, len);
    }

    void Initialize(sm::ServiceManager *SM)
    {
        u32 total = (sizeof(devices) / sizeof(inDevice));
        if(!fsinit)
        {
            sfsp = fsp::Initialize(SM).AssertOk();
            memset(devices, 0, sizeof(devices));
            for(u32 i = 0; i < total; i++)
            {
                memcpy(&devices[i].device, &devoptab, sizeof(devoptab));
                devices[i].device.name = devices[i].name;
                devices[i].id = i;
            }
            devdefault = -1;
            fsinit = true;
        }
    }

    static int inMountDevice(const char *dname, fsp::FileSystem *ifs, inDevice **out)
    {
        inDevice *dev = NULL;
        if(inFindDevice(dname)) return -1;
        if(!fsinit) return -1;
        dev = inFindDevice(NULL);
        if(dev == NULL) return -1;
        dev->fs = ifs; 
        memset(dev->name, 0, sizeof(dev->mame));
        strncpy(dev->name, name, sizeof(dev->name) - 1);
        int rdev = AddDevice(&dev->device);
        if(rdev == -1) return rdev;
        dev->setup = true;
        if(out) *out = dev;
        return rdev;
    }

    Result Mount(fsp::FileSystem *FS, const char *DeviceName)
    {
        return inMountDevice(DeviceName, FS, NULL);
    }

    static int inUnmountDevice(inDevice *dev)
    {
        char name[0x22];
        if(!dev->setup) return 0;
        memset(name, 0, sizeof(name));
        strncpy(name, dev->name, sizeof(name) - 2);
        strncat(name, ":", sizeof(name) - strlen(name) - 1);
        RemoveDevice(name);
        &dev->ifs->Close();
        if(dev->id == devdefault) devdefault = -1;
        if(dev->id == devcwd) devcwd = devdefault;
        dev->setup = 0;
        memset(dev->name, 0, sizeof(dev->name));
        return 0;
    }

    Result Unmount(const char *DeviceName)
    {
        inDevice *dev = inFindDevice(DeviceName);
        if(dev == NULL) return 1;
        return inUnmountDevice(dev);
    }

    Result MountSdCard(const char *DeviceName)
    {
        fsp::FileSystem *sdfs = sfsp->OpenSdCardFileSystem().AssertOk();
        return Mount(sdfs, DeviceName);
    }

    fsp::FileSystem *GetFileSystem(const char *DeviceName)
    {
        inDevice *dev = inFindDevice(DeviceName);
        if(dev == NULL) return NULL;
        return dev->fs;
    }

    Result UnmountAllDevices()
    {
        u32 total = (sizeof(devices) / sizeof(inDevice));
        if(!fsinit) return 0;
        for(u32 i = 0; i < total; i++) inUnmountDevice(&devices[i]);
    }

    static int inFS_open(struct _reent *r, void *fileStruct, const char *path, int flags, int mode)
    {
        fsp::File *fd;
        Result rc = 0;
        u32 dflags = 0;
        u32 attrs = 0;
        char fpath[4096];
        inDevice *dev = NULL;
        if(inGetFSPath(r, path, &dev, fpsth) == -1) return -1;
        inFile *inf = (inFile*)fileStruct;
        switch(flags & O_ACCMODE)
        {
            case O_RDONLY:
                dflags |= BIO_BITMASK(0);
                if(flags & O_APPEND)
                {
                    r->_errno = EINVAL;
                    return -1;
                }
                break;
            case O_WRONLY:

        }
    }

    static int inFS_close(struct _reent *r, void *fd);
    static ssize_t inFS_write(struct _reent *r, void *fd, const char *ptr, size_t len);
    static ssize_t inFS_write_safe(struct _reent *r, void *fd, const char *ptr, size_t len);
    static ssize_t inFS_read(struct _reent *r, void *fd, char *ptr, size_t len);
    static ssize_t inFS_read_safe(struct _reent *r, void *fd, char *ptr, size_t len);
    static off_t inFS_seek(struct _reent *r, void *fd, off_t pos, int dir);
    static int inFS_fstat(struct _reent *r, void *fd, struct stat *st);
    static int inFS_stat(struct _reent *r, const char *file, struct stat *st);
    static int inFS_link(struct _reent *r, const char *existing, const char *newLink);
    static int inFS_unlink(struct _reent *r, const char *name);
    static int inFS_chdir(struct _reent *r, const char *name);
    static int inFS_rename(struct _reent *r, const char *oldName, const char *newName);
    static int inFS_mkdir(struct _reent *r, const char *path, int mode);
    static DIR_ITER* inFS_diropen(struct _reent *r, DIR_ITER *dirState, const char *path);
    static int inFS_dirreset(struct _reent *r, DIR_ITER *dirState);
    static int inFS_dirnext(struct _reent *r, DIR_ITER *dirState, char *filename, struct stat *filestat);
    static int inFS_dirclose(struct _reent *r, DIR_ITER *dirState);
    static int inFS_statvfs(struct _reent *r, const char *path, struct statvfs *buf);
    static int inFS_ftruncate(struct _reent *r, void *fd, off_t len);
    static int inFS_fsync(struct _reent *r, void *fd);
    static int inFS_chmod(struct _reent *r, const char *path, mode_t mode);
    static int inFS_fchmod(struct _reent *r, void *fd, mode_t mode);
    static int inFS_rmdir(struct _reent *r, const char *name);
}