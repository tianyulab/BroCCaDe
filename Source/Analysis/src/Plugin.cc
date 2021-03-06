/*******************************************************************************\
* Copyright (c) 2017 Murdoch University                                         *
*                                                                               *
* Written by Hendra Gunadi (Hendra.Gunadi@murdoch.edu.au)                       *
*                                                                               *
* Redistribution and use in source and binary forms, with or without            *
* modification, are permitted provided that the following conditions are met:   *
*                                                                               *
* (1) Redistributions of source code must retain the above copyright            *
*     notice, this list of conditions and the following disclaimer.             *
*                                                                               *
* (2) Redistributions in binary form must reproduce the above copyright         *
*     notice, this list of conditions and the following disclaimer in           *
*     the documentation and/or other materials provided with the                *
*     distribution.                                                             *
*                                                                               *
* (3) Neither the name of Hendra Gunadi and/or Murdoch University, nor          *
*     the names of contributors may be used to endorse or promote               *
*     products derived from this software without specific prior written        *
*     permission.                                                               *
*                                                                               *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"   *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE     *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE    *
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF          *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS      *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       *
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE    *
* POSSIBILITY OF SUCH DAMAGE.                                                   *
*                                                                               *
* Plugin.cc : Implements Plugin.h                                               *
\*******************************************************************************/

#include "Plugin.h"
#include "analysis.bif.h"
#include "Bin_Strategy_Interval.h"  // abstract Bin allocator
#include <vector>                   // vector
#include <unordered_map>            // unordered_map
#include <iostream>			
#include <cfloat>                   // DBL_MAX
#include <fstream>                  // ifstream
#include <memory>                   // shared_ptr and unique_ptr
#include <algorithm>                // sort
#include "Flow.h"                   // Flow
#include "Analysis.h"
#include "KS_Flow.h"
#include "Entropy_Flow.h"
#include "CCE_Flow.h"
#include "Regularity_Flow.h"
#include "Autocorrelation_Flow.h"
#include "MultiModal_Flow.h"

//#define DEBUG_H

#ifdef DEBUG_H
#include <stdio.h>
#endif

using namespace BifEnum::FeatureAnalysis;

namespace plugin { namespace Analysis_FeatureAnalysis { Plugin plugin; } }

using namespace plugin::Analysis_FeatureAnalysis;

using namespace CCD;

plugin::Configuration Plugin::Configure()
	{
	plugin::Configuration config;
	config.name = "Analysis::FeatureAnalysis";
	config.description = "<Insert description>";
	config.version.major = 0;
	config.version.minor = 1;

	// initialize the config data for the flow analysis
	_flow_config = std::shared_ptr<FlowConfig> (new FlowConfig());

    // Bin strategies
	_flow_config->default_binner = std::shared_ptr<Bin_Strategy_Null> (new Bin_Strategy_Null(256)) ;
    _flow_config->binner.resize(NUMBER_OF_ANALYSIS);
    
	_flow_config->Autocorrelation_lags.resize(DEFAULT_SET_IDS, DEFAULT_NUM_LAG);    // AC lags
	_flow_config->KS_window_size.resize(DEFAULT_SET_IDS, DEFAULT_KS_WINDOW_SIZE);   // KS window
	_flow_config->Regularity_window_number.resize(DEFAULT_SET_IDS, DEFAULT_REGULARITY_WINDOW_NUMBER);  // Regularity window number
	_flow_config->Regularity_window_size.resize(DEFAULT_SET_IDS, DEFAULT_REGULARITY_WINDOW_SIZE);	// Regularity window size
	_flow_config->CCE_pattern_size.resize(DEFAULT_SET_IDS, DEFAULT_CCE_PATTERN_SIZE);   // CCE pattern size
    // default values for Set ID, number of analysis, and tag count
	_flow_config->tag_count = DEFAULT_TAG_COUNT;
	_flow_config->set_IDs = DEFAULT_SET_IDS;
	_flow_config->type_count = DATA_CONTAINER_TYPE_COUNT;
	_flow_config->number_of_analysis = NUMBER_OF_ANALYSIS;
	_flow_config->step_sizes.resize(DEFAULT_SET_IDS);                               // step sizes
	// hard coded for now: step size for URG analysis is for every packet
	_flow_config->step_sizes[0] = 1; 		
	// hard coded for now
	_flow_config->step_sizes[1] = DEFAULT_STEP_SIZE;
	// hard coded for now: step size for PTunnel analysis is for every ICMP payload
	_flow_config->step_sizes[2] = 1; 		

    // get the enumeration from analysis.bif
	_flow_config->KS_analysis = KS_ANALYSIS;
	_flow_config->Entropy_analysis = ENTROPY_ANALYSIS;
	_flow_config->CCE_analysis = CCE_ANALYSIS;
	_flow_config->Autocorrelation_analysis = AUTOCORRELATION_ANALYSIS;
	_flow_config->MultiModal_analysis = MULTIMODAL_ANALYSIS;
	_flow_config->Regularity_analysis = REGULARITY_ANALYSIS;
	_flow_config->Null_analysis = NULL_ANALYSIS;

    // mapping from analysis to its data container
	_flow_config->map_analysis_data.resize(NUMBER_OF_ANALYSIS);
	_flow_config->map_analysis_data[KS_ANALYSIS] = RAW_DATA;
	_flow_config->map_analysis_data[ENTROPY_ANALYSIS] = HISTOGRAM_DATA;
	_flow_config->map_analysis_data[CCE_ANALYSIS] = PATTERN_DATA;
	_flow_config->map_analysis_data[NULL_ANALYSIS] = NULL_DATA;
	_flow_config->map_analysis_data[MULTIMODAL_ANALYSIS] = HISTOGRAM_DATA;
	_flow_config->map_analysis_data[AUTOCORRELATION_ANALYSIS] = RAW_DATA;
	_flow_config->map_analysis_data[REGULARITY_ANALYSIS] = REGULARITY_DATA;

    _flow_config->null_data = std::shared_ptr<Null_Data> (new Null_Data());
    _flow_config->null_analysis = std::shared_ptr<NullAnalysis> (new NullAnalysis(_flow_config->null_data));

	return config;
	}

void plugin::Analysis_FeatureAnalysis::Plugin::Add_Normal_Data(Val* tag_val, StringVal* KS_data_file)
{
    int tag = tag_val->AsEnum();
	std::string file_name((const char*) KS_data_file->Bytes());

    // dynamically resize if the tag is outside of known tags (sanity check and resize for tag)
    if (tag >= _flow_config->KS_normal_data.size()) _flow_config->KS_normal_data.resize(tag+1);

    // initialize the normal data vector if it does not exists
    if (_flow_config->KS_normal_data[tag].get() == NULL) 
        _flow_config->KS_normal_data[tag] = std::shared_ptr<std::vector<std::vector<double>>>(
        new std::vector<std::vector<double>>());

    ssize_t idx = _flow_config->KS_normal_data[tag]->size();
    _flow_config->KS_normal_data[tag]->resize(idx + 1); // add the normal data to the list of normal data

	ifstream f;	        // input file stream
	std::string line;	// temporary string to read the file
	unsigned int N;		// number of intervals in the training data

	// reading series of feature from the input file
	// the format of Data_Container_Enum _map_analysis_data[]the file is
	// <number of data points>
	// <interarrival time>
	// ...
	f.open (file_name);
	if (!f.is_open()) {cout << "(data) can't open file" << file_name << "\n"; return;}
	
	if ( getline (f, line) )
	{
		N = stoul(line);
		for (unsigned int i = 0; i < N; i++)
		{
			getline(f, line);
			try { (*(_flow_config->KS_normal_data[tag]))[idx].push_back(stod(line)); }  // add data point
			catch (std::invalid_argument& e) { cout << e.what() << "\n"; }
		}
	}
	f.close();
    std::sort((*(_flow_config->KS_normal_data[tag]))[idx].begin(), (*(_flow_config->KS_normal_data[tag]))[idx].end());  // sort
}

void plugin::Analysis_FeatureAnalysis::Plugin::Set_Bin_Null
    (Val* tag_val, Val* aid_val, unsigned int bin_count)
{
    int tag = tag_val->AsEnum();
    std::vector<Val*>* aid_vector = aid_val->AsVector();
    for (unsigned int i = 0; i < aid_vector->size(); i++)
	{
		unsigned int aid = (*aid_vector)[i]->AsEnum();
        // sanity check and resize for tag
        if (tag >= _flow_config->binner[aid].size()) _flow_config->binner[aid].resize(tag+1, _flow_config->default_binner);
        // set the bin strategy for a specific analysis ID and tag
        _flow_config->binner[aid][tag] = std::shared_ptr<Bin_Strategy_Null> (new Bin_Strategy_Null(bin_count));
    }
}

void plugin::Analysis_FeatureAnalysis::Plugin::Load_Interval
    (Val* tag_val, Val* aid_val, StringVal* Interval_data_file)
{
    std::string file_name_interval((const char*) Interval_data_file->Bytes());
    std::shared_ptr<Bin_Strategy_Interval> temp_binner(new Bin_Strategy_Interval());
    // reading intervals from the input file
    // the format of the file is
    // <number of intervals>
    // <lower bound> <upper bound>
    // ...
    ifstream f;	                    // input file stream
    std::string line;	            // temporary string to read the file
    unsigned int N;		            // number of intervals in the training data

  	f.open (file_name_interval);
    if (!f.is_open()) {cout << "(data) can't open file" << file_name_interval << "\n"; return;}

    if ( getline (f, line) )
    {
	    N = stoul(line);
	    for (unsigned int i = 0; i < N; i++)
	    {
		    getline(f, line);
		    try
		    {
			    double left; 
			    double right; 
                // try to read 2 doubles from the line
			    if (sscanf(line.c_str(), "%lf %lf", &left, &right) < 2)
			    {
				    cout << "Plugin.cc: can't read 2 doubles from the line \"" << line << "\"\n";
				    continue;
			    }
			    temp_binner->add_interval(left, right);
		    }
		    catch (std::invalid_argument& e) { cout << e.what() << "\n"; }
	    }
    }
    f.close();

    // set the loaded intervals as the bin strategy for a particular analysis and tag
    int tag = tag_val->AsEnum();
    std::vector<Val*>* aid_vector = aid_val->AsVector();
    for (unsigned int i = 0; i < aid_vector->size(); i++)
	{
		unsigned int aid = (*aid_vector)[i]->AsEnum();
        // sanity check and resize for tag
        if (tag >= _flow_config->binner[aid].size()) _flow_config->binner[aid].resize(tag+1, _flow_config->default_binner);
        _flow_config->binner[aid][tag] = temp_binner;
    }
}

// whenever Bro decided to clean a connection, we also flush the connection
// from the dictionary
void plugin::Analysis_FeatureAnalysis::Plugin::RemoveConnection(StringVal* UID)
{
	std::string UID_str((const char*) UID->Bytes());
	_flow_dict.erase(UID_str);
}

void plugin::Analysis_FeatureAnalysis::Plugin::ConfigureInternalType()
{
    result_vector_type = internal_type("result_vector")->AsVectorType();
    analysis_result_type = internal_type("analysis_result")->AsRecordType();
    feature_vector_type = internal_type("FeatureAnalysis::feature_vector")->AsVectorType();
    direction_enum_type = internal_type("FeatureAnalysis::Direction")->AsEnumType();
}

// set the step size for all future Flow
void plugin::Analysis_FeatureAnalysis::Plugin::SetStepSize(Val* Set_ID, unsigned int step_size)
{
	int set_ID = Set_ID->AsEnum();
    // sanity check and resize for set ID
	if ((unsigned int) set_ID >= _flow_config->set_IDs) _flow_config->set_IDs = (unsigned int) set_ID + 1;
    if (_flow_config->step_sizes.size() < _flow_config->set_IDs) _flow_config->step_sizes.resize(_flow_config->set_IDs);

	_flow_config->step_sizes[set_ID] = step_size; // set the step size for a particular set ID
}

void plugin::Analysis_FeatureAnalysis::Plugin::Set_Regularity_Parameters(Val* Set_ID, unsigned int window_number, unsigned int window_size)
{
    int set_ID = Set_ID->AsEnum();
    // sanity check and resize for set ID
	if ((unsigned int) set_ID >= _flow_config->set_IDs) _flow_config->set_IDs = (unsigned int) set_ID + 1;
    if (_flow_config->Regularity_window_size.size() < _flow_config->set_IDs) 
        _flow_config->Regularity_window_size.resize(_flow_config->set_IDs);
    if (_flow_config->Regularity_window_number.size() < _flow_config->set_IDs) 
        _flow_config->Regularity_window_number.resize(_flow_config->set_IDs);

    // set the window size and window number for a particular set ID
    _flow_config->Regularity_window_size[set_ID] = window_size;
    _flow_config->Regularity_window_number[set_ID] = window_number;
}

void plugin::Analysis_FeatureAnalysis::Plugin::Set_KS_Window_Size(Val* Set_ID, unsigned int window_size)
{
    int set_ID = Set_ID->AsEnum();
    // sanity check and resize for set ID
	if ((unsigned int) set_ID >= _flow_config->set_IDs) 
		_flow_config->set_IDs = (unsigned int) set_ID + 1;
    if (_flow_config->KS_window_size.size() < _flow_config->set_IDs) 
        _flow_config->KS_window_size.resize(_flow_config->set_IDs);

    _flow_config->KS_window_size[set_ID] = window_size; // set the window size for a particular set ID
}

void plugin::Analysis_FeatureAnalysis::Plugin::Set_CCE_Pattern_Size(Val* Set_ID, unsigned int pattern_size)
{
    int set_ID = Set_ID->AsEnum();
    // sanity check and resize for set ID
	if ((unsigned int) set_ID >= _flow_config->set_IDs) 
		_flow_config->set_IDs = (unsigned int) set_ID + 1;
    if (_flow_config->CCE_pattern_size.size() < _flow_config->set_IDs) 
		_flow_config->CCE_pattern_size.resize(_flow_config->set_IDs);


    _flow_config->CCE_pattern_size[set_ID] = pattern_size; // set the CCE pattern size for a particular set ID
}

void plugin::Analysis_FeatureAnalysis::Plugin::Set_Autocorrelation_Lags(Val* Set_ID, unsigned int lag_max)
{
    int set_ID = Set_ID->AsEnum();
    // sanity check and resize for set ID
	if ((unsigned int) set_ID >= _flow_config->set_IDs) 
		_flow_config->set_IDs = (unsigned int) set_ID + 1;
    if (_flow_config->Autocorrelation_lags.size() < _flow_config->set_IDs) 
		_flow_config->Autocorrelation_lags.resize(_flow_config->set_IDs);

    _flow_config->Autocorrelation_lags[set_ID] = lag_max;   // set the lag max for a particular set ID
}

// register the request for analysis which is distinguished by its analysis ID and tag
void plugin::Analysis_FeatureAnalysis::Plugin::RegisterAnalysis(StringVal* UID, Val* Set_ID, Val* conn_ID, Val* direction_val)
{
	std::unique_ptr<std::string> UID_ptr(new std::string((const char*) UID->Bytes()));
    int set_ID = Set_ID->AsEnum();

    // sanity check and resize for set ID
	if ((unsigned int) set_ID >= _flow_config->set_IDs) 
	{
		_flow_config->set_IDs = (unsigned int) set_ID + 1;
		_flow_config->step_sizes.resize(_flow_config->set_IDs);
		_flow_config->step_sizes[set_ID] = DEFAULT_STEP_SIZE;
	}

    // hold internals of conn_id
    RecordVal* conn_r = conn_ID->AsRecordVal();
    std::unique_ptr<IPAddr> temp_src_ip(new IPAddr(conn_r->Lookup(0)->AsAddr()));
    _current_src_ip.swap(temp_src_ip);
    PortVal* src_port = conn_r->Lookup(1)->AsPortVal();
    _current_src_port = src_port->Port();
    _current_src_proto = src_port->PortType();
    std::unique_ptr<IPAddr> temp_dst_ip(new IPAddr(conn_r->Lookup(2)->AsAddr()));
    _current_dst_ip.swap(temp_dst_ip);
    PortVal* dst_port = conn_r->Lookup(3)->AsPortVal();
    _current_dst_port = dst_port->Port();
    _current_dst_proto = dst_port->PortType();

    // hold current set ID and direction
	_current_set_ID = set_ID;
    _current_direction = direction_val->AsEnum();

    // find the bidirectional flow
	std::unordered_map <std::string, std::shared_ptr<BiFlow> >::iterator 
			got = _flow_dict.find(*UID_ptr);

	std::shared_ptr<Flow> flow;
	// check if we have the flow in the dictionary
	if (got == _flow_dict.end())
	{
#ifdef DEBUG_H
		printf("creating new flow ID: %s\n", UID_ptr->c_str());
#endif
		// if not then create a new entry
        std::shared_ptr<BiFlow> biflow(new BiFlow(_flow_config));

		pair<std::unordered_map<std::string, std::shared_ptr<BiFlow> >::iterator, bool>
			result = _flow_dict.emplace(*UID_ptr, biflow);
		
		// and check that the entry is created successfully
		if (!result.second) 
			printf("error in creating new analysis per flow\n");

		flow = biflow->get(_current_direction);
	}
	else
	{ 
		flow = got->second->get(_current_direction);
	}

    // set the current temporary references
	_current_flow = flow;

	// indicate the flow begin
	flow->begin_adding_feature(set_ID);
}

// register the request for analysis which is distinguished by its analysis ID and tag
void plugin::Analysis_FeatureAnalysis::Plugin::AddFeature(double feature,
	Val* aid_val, Val* tag_val)
{
	int tag = tag_val->AsEnum();

	// sanity check for tag
	if (tag < 0)
	{
		printf("error: invalid Tag %d\n", tag);
		return;
	}

	if ((unsigned int) tag >= _flow_config->tag_count) 
        _flow_config->tag_count = (unsigned int) tag + 1;

	// get the analysis ID from the aid_val
	std::vector<Val*>* aid_vector_val = aid_val->AsVector();
	std::vector<unsigned int> aid_vector(aid_vector_val->size());
	for (unsigned int i = 0; i < aid_vector_val->size(); i++)
	{
		unsigned int aid = (*aid_vector_val)[i]->AsEnum();
		// sanity check for aid
		if (aid < 0 || aid > _flow_config->number_of_analysis)
		{
			printf("error: invalid analysis ID %d\n", aid);
			return;
		}
		aid_vector[i] = aid;
	}

	// add the feature to the current flow
	_current_flow->add_feature(tag, aid_vector, feature);
}


// Act upon the analysis request
void plugin::Analysis_FeatureAnalysis::Plugin::CalculateMetric()
{
	unsigned int index;
	bool metric_calculated = _current_flow->end_adding_feature();

	// send the actual event if metric calculation is triggered
	if (metric_calculated) 
	{
		// get the list of metrics from the Flow object
		std::vector<std::unique_ptr<CCD::TempValue>> return_list = _current_flow->get_result();

		val_list* vl = new val_list;
		vl->append(new Val(_current_set_ID, TYPE_ENUM));
        vl->append(new Val(_current_direction, TYPE_ENUM));
		vl->append(new VectorVal(result_vector_type));
		// add the metric result to the vector for event argument
		for (unsigned long i = 0; i < return_list.size(); i++)
		{
			RecordVal* r = new RecordVal(analysis_result_type);
			r->Assign(0, new Val(return_list[i]->_value, TYPE_DOUBLE));
			r->Assign(1, new Val(return_list[i]->_tag, TYPE_ENUM));
			r->Assign(2, new Val(return_list[i]->_aid, TYPE_ENUM));
			(*vl)[2]->AsVectorVal()->Assign(i, r);
		}
        // Build the connection ID
        RecordVal* conn_ID = new RecordVal(conn_id);
		conn_ID->Assign(0, new AddrVal(*_current_src_ip));
		conn_ID->Assign(1, new PortVal(_current_src_port, _current_src_proto));
		conn_ID->Assign(2, new AddrVal(*_current_dst_ip));
		conn_ID->Assign(3, new PortVal(_current_dst_port, _current_dst_proto));
        vl->append(conn_ID);

		mgr.QueueEvent(FeatureAnalysis::metric_event, vl);  // queue the event
	}
}

EnumVal* plugin::Analysis_FeatureAnalysis::Plugin::GetDirection(Val* conn_source, Val* pkt_source)
{
    std::unique_ptr<IPAddr> Conn_ip(new IPAddr(conn_source->AsAddr())); // Bro connection source
    std::unique_ptr<IPAddr> Pkt_ip(new IPAddr(pkt_source->AsAddr()));   // Packet source
    if (*Conn_ip == *Pkt_ip) return new EnumVal(0, direction_enum_type);
    return new EnumVal(1, direction_enum_type);
}

// extract the value from the vector based on the analysis id and input tag
Val* plugin::Analysis_FeatureAnalysis::Plugin::Extract(Val* v, Val* aid, Val* tag)
{
	unsigned int i;
	unsigned int n = v->AsVectorVal()->Size();
	for (i = 0; i < n; i++)
	{
		RecordVal* e = (v->AsVectorVal())->Lookup(i)->AsRecordVal();
		if ((e->Lookup("tag")->AsEnum() == tag->AsEnum()) && 
				(e->Lookup("aid")->AsEnum() == aid->AsEnum())) 
			return new Val(e->Lookup("value")->AsDouble(), TYPE_DOUBLE);
	}
	return new Val(-DBL_MAX, TYPE_DOUBLE);
}

// strip all other information and just represent the metric vector
Val* plugin::Analysis_FeatureAnalysis::Plugin::ExtractVector(Val* v)
{
	unsigned int i;
	unsigned int n = v->AsVectorVal()->Size();
	VectorVal* rv = new VectorVal(feature_vector_type);
	for (i = 0; i < n; i++)
	{
		RecordVal* e = (v->AsVectorVal())->Lookup(i)->AsRecordVal();
		rv->Assign(i, new Val(e->Lookup("value")->AsDouble(), TYPE_DOUBLE));
	}
	return rv;
}
