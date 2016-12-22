#include "ethernet.h"
#include "syshead.h"
#include "skbuff.h"

inline struct eth_hdr *eth_hdr(struct sk_buff *skb)
{
    struct eth_hdr *hdr = (struct eth_hdr *)skb_head(skb);

    hdr->ethertype = ntohs(hdr->ethertype);

    return hdr;
}
