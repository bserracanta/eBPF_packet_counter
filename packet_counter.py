from bcc import BPF
from bcc.utils import printb

device = "ens224"
b = BPF(src_file="packet_counter.c")
fn = b.load_func("packet_counter", BPF.XDP)
b.attach_xdp(device, fn, 0)

try:
    b.trace_print()
except KeyboardInterrupt:

    dist = b.get_table("table")
    for k, v in dist.items():
        print("Tuple : %10d, COUNT : %10d" % (k.value, v.value))

b.remove_xdp(device, 0)
