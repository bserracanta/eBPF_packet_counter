#define KBUILD_MODNAME "packet_counter"
#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <time.h>

#define THRESHOLD 10

struct packetInfo {
    be32 saddr;
    be32 daddr;
    u8 proto;
    unsigned int sport;
     unsigned int dport;
};

struct value {
    timespec timestamp;
    int counter;
}

static const struct packetInfo emptyPacket;
static const struct value emptyValue;

BPF_HASH(table, packetInfo);

int packet_counter(struct xdp_md *ctx){

    struct packetInfo packet;
    struct value val;
    bpf_trace_printk("packet received\n");

    struct timespec current_time;
    clock_gettime(CLOCK_REALTIME, &current_time);
    val.timestamp = current_time;

    void *data = (void *)(long)ctx->data;
    void *data_end = (void *)(long)ctx->data_end;
    struct ethhdr *eth = data;

    if ((void *)eth + sizeof(*eth) <= data_end)
    {

        struct iphdr *ip = data + sizeof(*eth);
        if ((void *)ip + sizeof(*ip) <= data_end)
        {
            packet.saddr = ip->saddr;
            packet.daddr = ip->daddr;
            packet.proto = ip->protocol;
        

            if (ip->protocol == IPPROTO_UDP)
            {

                struct udphdr *udp = (void *)ip + sizeof(*ip);
                if ((void *)udp + sizeof(*udp) <= data_end)
                {
                    //htons() modifies byte order (Endianness), having the most significant first
                    // host-to-network short byte order translation

                    packet.sport = htons((unsigned short int) udp->source); //sport now has the source port
                    packet.dport = htons((unsigned short int) udp->dest); 
                }
            }
            else if (ip->protocol == IPPROTO_TCP)
            {
                //tcp_header = (struct tcphdr *)((__u32 *)ip + ip->ihl); //this fixed the problem
                struct tcphdr *tcp = (void *)ip + sizeof(*ip);
                if ((void *)tcp + sizeof(*tcp) <= data_end)
                {
                    packet.sport = htons((unsigned short int) tcp->source); //sport now has the source port
                    packet.dport = htons((unsigned short int) tcp->dest);   //dport now has the dest port
                }
            }
            
        }
        struct value *packet_value_pointer = NULL;
        packet_value_pointer = table.lookup(packet);

        if (packet_value_pointer == NULL){
            val.counter = 0;
            table.insert(packet, value)
        }else{
            *packet_value_pointer.timestamp = val.timestamp;
            *packet_value_pointer.counter = *packet_value_pointer.counter + 1;
            if (*packet_value_pointer.counter > THRESHOLD){
                ip->tos = 0x08;
            }
        }
        // check if in the table
            // if not add to table 
            // if yes +1 packet count, update timestamp, check count
                // if count bigger than threshold, tag with different ToS: 
                    //ip->tos assignar IPTOS_THROUGHPUT= 0x08 o IPTOS_LOWDELAY si fem tag dels curts
                // else pass
    }
    // it is necessary to do this cleanup of the structs?? or when a new packet arrives a new struct will be created and it will be overwriten?
    packet = emptyPacket;
    val = emptyValue;

    return XDP_PASS;
}

// https://github.com/iovisor/bcc/blob/master/docs/reference_guide.md#20-maplookup_or_try_init 