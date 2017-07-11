#@load /opt/bro/lib/bro/plugins/Feature_IAT/lib/bif

#type analysis_tag : enum {INTERARRIVAL_TIME, PACKET_LENGTH, URG_FLAG, URG_POINTER, ICMP_PAYLOAD_4_BYTES, TTL};

event connection_state_remove (c: connection)
{
	BinTraining::RemoveConn(c$uid, TTL);
}

#event IAT::feature_event(UID:string, id:conn_id, value: double)
#{
#	BinTraining::add_feature(UID, value, INTERARRIVAL_TIME);
#}

event TTL_feature_event(UID:string, id:conn_id, direction: BinTraining::Direction, value: double) {
    BinTraining::add_feature(UID, direction, value, TTL);
}

function get_direction(a : addr, b : addr) : BinTraining::Direction
{
    if (a == b) return BinTraining::FORWARD;
    return BinTraining::BACKWARD;
}


event new_packet (c: connection, p: pkt_hdr)
{
	if ( p ?$ ip )
	{
		event TTL_feature_event(c$uid, c$id, get_direction(p$ip$src, c$id$orig_h) , p$ip$ttl);
	}
}


