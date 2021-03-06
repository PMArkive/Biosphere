
#pragma once
#include <bio/root.hpp>
#include <bio/biotope/arm.hpp>

namespace bio::svc
{
    u32 SetHeapSize(void **OutAddress, u64 Size);
    u32 SetMemoryPermission(void *StartAddress, u64 Size, u32 Permissions);
    u32 SetMemoryAttribute(void *StartAddress, u64 Size, u32 State0, u32 State1);
    u32 MapMemory(void *DestAddress, void *SourceAddress, u64 Size);
    u32 UnmapMemory(void *DestAddress, void *SourceAddress, u64 Size);
    u32 QueryMemory(bio::MemoryInfo *MemInfo, u32 *PageInfo, u64 Address);
    void BIO_NORETURN ExitProcess();
    u32 CreateThread(u32 *Out_ThreadHandle, void *EntryPoint, void *Arguments, void *StackTop, int Priority, int CPUID);
    u32 StartThread(u32 ThreadHandle);
    void BIO_NORETURN ExitThread();
    u32 SleepThread(u64 NanoSeconds);
    u32 GetThreadPriority(u32 *Out_Priority, u32 ThreadHandle);
    u32 SetThreadPriority(u32 ThreadHandle, u32 Priority);
    u32 GetThreadCoreMask(s32 *Out_PreferredCore, u32 *Out_AffinityMask, u32 ThreadHandle);
    u32 SetThreadCoreMask(u32 ThreadHandle, s32 PreferredCore, u32 AffinityMask);
    u32 GetCurrentProcessorNumber();
    u32 SignalEvent(u32 EventHandle);
    u32 ClearEvent(u32 EventHandle);
    u32 MapSharedMemory(u32 VarHandle, void *Address, size_t Size, u32 Permissions);
    u32 UnmapSharedMemory(u32 VarHandle, void *Address, size_t Size);
    u32 CreateTransferMemory(u32 *Out_Handle, void *Address, size_t Size, u32 Permissions);
    u32 CloseHandle(u32 VarHandle);
    u32 ResetSignal(u32 VarHandle);
    u32 WaitSynchronization(s32 *Index, const u32 *Handles, s32 HandleCount, u64 Timeout);
    u32 CancelSynchronization(u32 ThreadHandle);
    u32 ArbitrateLock(u32 WaitTag, u32 *TagLocation, u32 SelfTag);
    u32 ArbitrateUnlock(u32 *TagLocation);
    u32 WaitProcessWideKeyAtomic(u32 *Key, u32 *TagLocation, u32 SelfTag, u64 Timeout);
    u32 SignalProcessWideKey(u32 *Key, s32 Number);
    u64 GetSystemTick();
    u32 ConnectToNamedPort(u32 *Out_IPCSession, const char *Name);
    u32 SendSyncRequest(u32 IPCSession);
    u32 SendSyncRequestWithUserBuffer(void *UserBuffer, u64 Size, u32 IPCSession);
    u32 SendAsyncRequestWithUserBuffer(u32 *Handle, void *UserBuffer, u64 Size, u32 IPCSession);
    u32 GetProcessId(u64 *Out_ProcessID, u32 VarHandle);
    u32 GetThreadId(u64 *Out_ThreadID, u32 VarHandle);
    u32 Break(u32 BreakReason, u64 Param1, u64 Param2);
    u32 OutputDebugString(const char *String, u64 Size);
    void BIO_NORETURN ReturnFromException(u32 Code);
    u32 GetInfo(u64 *Out_Info, u64 FirstID, u32 VarHandle, u64 SecondID);
    u32 MapPhysicalMemory(void *Address, u64 Size);
    u32 UnmapPhysicalMemory(void *Address, u64 Size);
    u32 GetResourceLimitLimitValue(u64 *Out_Value, u32 ResourceLimit, bio::LimitableResource Which);
    u32 GetResourceLimitCurrentValue(u64 *Out_Value, u32 ResourceLimit, bio::LimitableResource Which);
    u32 SetThreadActivity(u32 ThreadHandle, bool Pause);
    u32 GetThreadContext3(bio::arm::ThreadContext *Context, u32 ThreadHandle);
    u32 CreateSession(u32 *ServerHandle, u32 *ClientHandle, u32 Unknown0, u64 Unknown1);
    u32 AcceptSession(u32 *SessionHandle, u32 PortHandle);
    u32 ReplyAndReceive(s32 *Index, const u32 *Handles, s32 HandleCount, u32 ReplyTarget, u64 Timeout);
    u32 ReplyAndReceiveWithUserBuffer(s32 *Index, void *UserBuffer, u64 Size, const u32 *Handles, s32 HandleCount, u32 ReplyTarget, u64 Timeout);
    u32 CreateEvent(u32 *ServerHandle, u32 *ClientHandle);
    u32 MapPhysicalMemoryUnsafe(void *Address, u64 Size);
    u32 UnmapPhysicalMemoryUnsafe(void *Address, u64 Size);
    u32 SetUnsafeLimit(u64 Size);
    u32 CreateCodeMemory(u32 *Out_CodeHandle, void *SourceAddress, u64 Size);
    u32 ControlCodeMemory(u32 CodeHandle, bio::CodeMapOperation MapOperation, void *DestAddress, u64 Size, u64 Permissions);
    u32 ReadWriteRegister(u32 *Out_Value, u64 RegisterAddress, u32 ReadWriteMask, u32 Value);
    u32 CreateSharedMemory(u32 *Out_Handle, size_t Size, u32 LocalPermissions, u32 OtherPermissions);
    u32 MapTransferMemory(u32 TransferMemory, void *Address, size_t Size, u32 Permissions);
    u32 UnmapTransferMemory(u32 TransferMemory, void *Address, size_t Size);
    u32 CreateInterruptEvent(u32 *Out_Event, u64 IRQNumber, u32 Flags);
    u32 QueryPhysicalAddress(u64 Out_Information[3], u64 VirtualAddress);
    u32 QueryIoMapping(u64 *Out_VirtualAddress, u64 PhysicalAddress, u64 Size);
    u32 CreateDeviceAddressSpace(u32 *Out_AddressSpace, u64 DevAddress, u64 DevSize);
    u32 AttachDeviceAddressSpace(u64 Device, u32 AddressSpace);
    u32 DetachDeviceAddressSpace(u64 Device, u32 AddressSpace);
    u32 MapDeviceAddressSpaceByForce(u32 DeviceAddress, u32 ProcessHandle, u64 MapAddress, u64 DevSize, u64 DevAddress, u32 Permissions);
    u32 MapDeviceAddressSpaceAligned(u32 DeviceAddress, u32 ProcessHandle, u64 MapAddress, u64 DevSize, u64 DevAddress, u32 Permissions);
    u32 UnmapDeviceAddressSpace(u32 DeviceAddress, u32 ProcessHandle, u64 MapAddress, u64 MapSize, u64 DevAddress);
    u32 DebugActiveProcess(u32 *ProcessHandle, u64 ProcessID);
    u32 BreakDebugProcess(u32 ProcessHandle);
    u32 GetDebugEvent(u8 *Out_Event, u32 ProcessHandle);
    u32 ContinueDebugEvent(u32 ProcessHandle, u32 Flags, u64 *TitleIDs, u32 TitleIDCount);
    u32 LegacyContinueDebugEvent(u32 ProcessHandle, u32 Flags, u64 ThreadID);
    u32 GetProcessList(u32 *Out_ProcessCount, u64 *Out_ProcessIDs, u32 MaxProcessIDs);
    u32 GetThreadList(u32 *Out_ThreadCount, u64 *Out_TitleIDs, u32 MaxTitleIDs, u32 ProcessHandle);
    u32 GetDebugThreadContext(bio::arm::ThreadContext *Out_Context, u32 ProcessHandle, u64 ThreadID, u32 Flags);
    u32 SetDebugThreadContext(u32 ProcessHandle, u64 ThreadID, const bio::arm::ThreadContext *Out_Context, u32 Flags);
    u32 QueryDebugProcessMemory(bio::MemoryInfo *MemInfo, u32 *PageInfo, u32 ProcessHandle, u64 Address);
    u32 ReadDebugProcessMemory(void *Out_Buffer, u32 ProcessHandle, u64 Address, u64 Size);
    u32 WriteDebugProcessMemory(u32 ProcessHandle, void *Buffer, u64 Address, u64 Size);
    u32 GetDebugThreadParam(u64 *Out_64Bit, u32 *Out_32Bit, u32 ProcessHandle, u64 ThreadID, bio::DebugThreadParameter ThreadParam);
    u32 GetSystemInfo(u64 *Out_Info, u64 FirstID, u32 VarHandle, u64 SecondID);
    u32 CreatePort(u32 *PortHandle, u32 *ClientHandle, s32 MaxSessions, bool IsLight, const char *Name);
    u32 ManageNamedPort(u32 *PortHandle, const char *Name, s32 MaxSessions);
    u32 ConnectToPort(u32 *IPCSession, u32 PortHandle);
    u32 SetProcessMemoryPermission(u32 ProcessHandle, u64 Address, u64 Size, u32 Permissions);
    u32 MapProcessMemory(void *DestAddress, u32 ProcessHandle, u64 SourceAddress, u64 Size);
    u32 UnmapProcessMemory(void *DestAddress, u32 ProcessHandle, u64 SourceAddress, u64 Size);
    u32 MapProcessCodeMemory(u32 ProcessHandle, u64 DestAddress, u64 SourceAddress, u64 Size);
    u32 UnmapProcessCodeMemory(u32 ProcessHandle, u64 DestAddress, u64 SourceAddress, u64 Size);
    u32 CreateProcess(u32 *Out_Process, void *ProcessInfo, u32 *Caps, u64 CapCount);
    u32 StartProcess(u32 ProcessHandle, s32 MainPriority, s32 DefaultCPU, u32 StackSize);
    u32 TerminateProcess(u32 ProcessHandle);
    u32 GetProcessInfo(u64 *Out_Value, u32 ProcessHandle, bio::ProcessInfo InfoType);
    u32 CreateResourceLimit(u32 *ResourceLimit);
    u32 SetResourceLimitLimitValue(u32 ResourceLimit, bio::LimitableResource Resource, u64 Value);
    u64 CallSecureMonitor(bio::SecureMonitorArgs *Args);
}