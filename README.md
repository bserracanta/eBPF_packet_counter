# eBPF_packet_counter #

[Summary from this project](https://gist.github.com/satrobit/17eb0ddd4e122425d96f60f45def9627)

[Interesting explanation: part 1](https://www.ferrisellis.com/content/ebpf_past_present_future/)

[Interesting explanation: Part 2](https://www.ferrisellis.com/content/ebpf_syscall_and_maps/)

### XDP Actions ###

Express Data Path (XDP) provides a data path which we can intercept or even edit packets using an eBPF program. 


Action  | Definition
-------------  | -------------
XDP_PASS       | Let the packet continue through the network stack
XDP_DROP       | Silently drop the packet
XDP_ABORTED    | Drop the packet with trace point exception
XDP_TX         | Bounce the packet back to the same NIC it arrived on
XDP_REDIRECT   | Redirect the packet to another NIC or user space socket via the AF_XDP address family


### XDP Execution ###

1. __Offloaded - NIC__: eBPF program can be offloaded to the network card itself, provided that the card supports XDP offloading.
2. __Native - NIC Driver__: eBPF will fallback to the driver if your card doesn't support offloading. The good news is that most drivers support this and performance is still impressive since this is all before entering the Linux network stack.
3. __Generic - Linux Network Stack__: This is the last option if the mentioned methods are not supported. Performance is not as good since the packet has entered the network stack.

### BPF Maps: ###

1. BPF_TABLE
2. BPF_HASH
3. BPF_ARRAY
4. BPF_HISTOGRAM
5. BPF_PERF_ARRAY

### BCC ###

BPF Compiler Collection (BCC) is a toolkit for creating eBPF programs with bindings to a few languages like Python, Lua and etc. 

Very useful for providing statistics after stopping the eBPF program execution.

----

## Execution ##

1. Install BCC dependency: `sudo apt-get install bpfcc-tools linux-headers-$(uname -r)`
2. Run `sudo pyhton3 packet_counter.py`
3. Send UDP packets to the interface specified at line 4 in packet_counter.py to different ports: `nc -u ip_addr_desired_iface port`
4. Interrupt program and the total count of packets/port will be displayed


