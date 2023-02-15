#include "queue.h"
#include "skel.h"

int rtable_size;	// the number of lines in the routing table
struct route_table_entry *rtable;

int arptable_size;	// the number of lines in the arp table
struct arp_entry *arptable;

// this function resets and recalculates the checksum
void reset_and_recalculate_ipchecksum(struct iphdr *dest_ip) {
	dest_ip->check = 0;
	dest_ip->check = ip_checksum((uint8_t *) dest_ip, sizeof(struct iphdr));
}

// this function is used by the function below to sort the entries in the  
// routing table with the same prefix in descending order after the mask
int sort_after_mask(uint32_t fm, uint32_t sm) {
	if(fm == sm)	return 0;
	return ntohl(sm) - ntohl(fm);
}

// this function is used by qsort to sort the entries in the routing table in descending 
// order after the prefix and calls the function above for a descending order after the mask
int sort_after_prefix(const void *f, const void *s) {
	if((*(struct route_table_entry *)f).prefix == (*(struct route_table_entry *)s).prefix) {
		return sort_after_mask((*(struct route_table_entry *)f).mask, (*(struct route_table_entry *)s).mask);
	}
	return ntohl((*(struct route_table_entry *)s).prefix) - ntohl((*(struct route_table_entry *)f).prefix);
}

// this function searches for the highest mask 
// for a prefix that matched the binary search
int prefix_with_max_mask(struct iphdr *dest_ip, int middle) {
	while((dest_ip->daddr & rtable[middle - 1].mask) == rtable[middle - 1].prefix) {
		middle -=1;	// it goes up in the routing table in order to find bigger masks
		if(middle == 0)	return middle;
	}
	return middle;
}

// this function is a binary search for the best entry in the routing table
// *my_max keeps track of the index of the best current entry at each step
void my_efficient_search(struct iphdr *dest_ip, int *my_max, int left, int right) {
	if(left <= right) {
		int middle = left + (right - left) / 2;

		if((dest_ip->daddr & rtable[middle].mask) == rtable[middle].prefix) {
			*my_max = prefix_with_max_mask(dest_ip, middle);
			// it keeps looking for a better match higher in the routing table
			my_efficient_search(dest_ip, my_max, left, (*my_max) - 1);
		}

		if((ntohl(dest_ip->daddr)) > ntohl(rtable[middle].prefix)) {
			my_efficient_search(dest_ip, my_max, left, middle - 1);
		}

		if((ntohl(dest_ip->daddr)) < ntohl(rtable[middle].prefix)) {
			my_efficient_search(dest_ip, my_max, middle + 1, right);
		}
	}
}

// this function is expecting the index of the best entry in  
// the routing table and returns that line from the routing table
struct route_table_entry *my_get_best_route(struct iphdr *dest_ip) {
	int index_in_rtable = -1;
	my_efficient_search(dest_ip, &(index_in_rtable), 0, rtable_size - 1);
	if(index_in_rtable != -1) return &rtable[index_in_rtable];
	return NULL;	// no entry was found
}

// this function is looking for the required entry in the arp table
struct arp_entry *get_arp_entry(uint32_t dest_ip) {
    for (size_t i = 0; i < arptable_size; i++) {
        if (dest_ip == arptable[i].ip) return &arptable[i];   	
    }
    return NULL; // no entry was found
}

int main(int argc, char *argv[])
{
	packet m;
	int rc;

	// Do not modify this line
	init(argc - 2, argv + 2);

	rtable = malloc(sizeof(struct route_table_entry) * 80000);
	DIE(rtable == NULL, "rtable");
	rtable_size = read_rtable(argv[1],rtable);

	// after reading the routing table, I sort it in descending order after the
	// prefix and again in descending order after the mask for equal prefixes
	qsort(rtable, rtable_size, sizeof(struct route_table_entry), sort_after_prefix);

	arptable = malloc(sizeof(struct  arp_entry) * 80000);
	DIE(arptable == NULL, "arptable");
	arptable_size = parse_arp_table("arp_table.txt", arptable);

	while (1) {
		rc = get_packet(&m);
		DIE(rc < 0, "get_packet");
		
		struct ether_header *eth = (struct ether_header *) m.payload;
		struct iphdr *iph = (struct iphdr *) (m.payload + sizeof(struct ether_header));
		
		uint16_t my_checksum = iph->check;		// it keeps the checksum
		reset_and_recalculate_ipchecksum(iph);	// recalculates the checksum

		if(my_checksum != iph->check) continue;

		if(iph->ttl == 0) continue;
		iph->ttl--;								// decrements time to live

		reset_and_recalculate_ipchecksum(iph);	// recalculates the checksum

		struct route_table_entry *my_best_route = my_get_best_route(iph);
		if (my_best_route == NULL) continue;

		struct arp_entry *arp = get_arp_entry(my_best_route->next_hop);
		if (arp == NULL) continue;

		// completes the destination with the found mac
		memcpy(eth->ether_dhost, arp->mac, 6);
		get_interface_mac(my_best_route->interface, eth->ether_shost);
		m.interface = my_best_route->interface;
		send_packet(&m);
	}
}