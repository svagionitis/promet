#ifndef _HEADER_FUNCTION_H
#define _HEADER_FUNCTION_H

void analyze(void *, size_t);
void process_tcp_data(void *, size_t);
void process_udp_data(void *, size_t);
void process_icmp_data(void *, size_t);
void process_ip(void *buffer);
search_data find(unsigned int ip);
unsigned int insert(unsigned int, int);
void detail_ip(unsigned int, struct iphdr *);

void analyze(void *buffer, size_t size_of_buffer){
    count++;
    struct iphdr *iph = (struct iphdr *)buffer;
    switch(iph->protocol){
        case IPPROTO_ICMP:
            process_icmp_data(buffer, size_of_buffer);
            break;
        case IPPROTO_TCP:
            process_tcp_data(buffer, size_of_buffer);
            break;
        case IPPROTO_UDP:
            process_udp_data(buffer, size_of_buffer);
            break;
        default:
            printf("Cannot identify the protocol for this connection.\n");
    }
}

void process_tcp_data(void *buffer, size_t size){
    struct iphdr *iph = (struct iphdr *)buffer;
    struct tcphdr *tcp = (struct tcphdr *)(buffer + iph->ihl);

    process_ip(buffer);
    printf("TCP %u\n", ntohs(tcp->source));
}

void process_udp_data(void *buffer, size_t size){

}

void process_icmp_data(void *buffer, size_t size){

}

void process_ip(void *buffer){
    struct iphdr *iph = (struct iphdr *)buffer;
    unsigned int ip, index;
    search_data data;

    //Empty it everytime.
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
    ip  = source.sin_addr.s_addr;

    data = find(ip);
    index = data.index;
    switch(data.found){
        case 0:
            index = insert(ip, data.index);
        case 1:
            // detail_ip(index, iph);
            break;
    }
    printf("%u\t", (source.sin_addr.s_addr));
    printf("%s\t", inet_ntoa(source.sin_addr));
    printf("%u\t", count);
}

search_data find(unsigned int ip){
    search_data data;
    int first, mid, last, i;
    first = 0;
    last = size_of_traffic - 1;
    mid = (first + last)/2;
    while(first <= last){
        if(traffic[mid].name_be == ip){
            data.found = 1;
            data.index = mid;
            return data;
        }
        if(traffic[mid].name_be > ip)
            last = mid;
        else if(traffic[mid].name_be < ip)
            first = mid;
        mid = (first+last)/2;
    }
    data.index = mid;
    data.found = 0;
    return data;
};

unsigned int insert(unsigned int ip, int index){
    int i;
    Traffic T;
    T.name_be = ip;
    T.tcp_counter = T.udp_counter = T.data_processed_tcp = T.data_processed_udp = 0;

    if(size_of_traffic == 0 || size_of_traffic == index){
        traffic[size_of_traffic] = T;
        size_of_traffic++;
        return size_of_traffic-1;
    }

    for(i=size_of_traffic; i>index; i--){
        traffic[i] = traffic[i-1];
    }
    size_of_traffic++;

    if(traffic[index].name_be > ip){
        traffic[index+1] = T;
        return index+1;
    }
    else{
        traffic[index] = T;
        return index;
    }

}


#endif /* _HEADER_FUNCTION_H */