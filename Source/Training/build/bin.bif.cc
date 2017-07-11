// This file was automatically generated by bifcl from /home/hendra/CCD/Source/Training/src/bin.bif (plugin mode).


#include "bin.bif.h"


#line 40 "bin.bif"
 #include "Plugin.h"
typedef plugin::Training_Bin::Plugin plugin_t;
static plugin_t *plugin_ref = &plugin::Training_Bin::plugin;

#line 45 "bin.bif"
#ifndef AUX_PLUGIN_DIRECTION_ENUM
#define AUX_PLUGIN_DIRECTION_ENUM
namespace BifType { namespace Enum{ namespace BinTraining {  EnumType * Direction;  } } }

#line 54 "bin.bif"
#endif

#line 57 "bin.bif"
Val* BifFunc::BinTraining::bro_RemoveConn(Frame* frame, val_list* BiF_ARGS)
	
#line 58 "bin.bif"
{
	if ( BiF_ARGS->length() != 2 )
		{
		reporter->Error("BinTraining::RemoveConn() takes exactly 2 argument(s)");
		return 0;
		}
	StringVal* UID = (StringVal*) ((*BiF_ARGS)[0]->AsStringVal());
	Val* tag = (Val*) ((*BiF_ARGS)[1]);

#line 58 "bin.bif"

		plugin_ref->RemoveConnection(UID, tag);
		return new Val(1, TYPE_BOOL);
	} // end of BifFunc::BinTraining::bro_RemoveConn

#line 61 "bin.bif"

#line 63 "bin.bif"
Val* BifFunc::BinTraining::bro_add_feature(Frame* frame, val_list* BiF_ARGS)
	
#line 64 "bin.bif"
{
	if ( BiF_ARGS->length() != 4 )
		{
		reporter->Error("BinTraining::add_feature() takes exactly 4 argument(s)");
		return 0;
		}
	StringVal* UID = (StringVal*) ((*BiF_ARGS)[0]->AsStringVal());
	Val* direction = (Val*) ((*BiF_ARGS)[1]);
	double feature = (double) ((*BiF_ARGS)[2]->AsDouble());
	Val* tag = (Val*) ((*BiF_ARGS)[3]);

#line 64 "bin.bif"

		plugin_ref->add_feature(UID, direction, feature, tag);
		return new Val(1, TYPE_BOOL);
	} // end of BifFunc::BinTraining::bro_add_feature

#line 67 "bin.bif"
