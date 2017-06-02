// This file was automatically generated by bifcl from /home/hendra/Experiment/IAT_Analysis/src/analysis.bif (plugin mode).

#if defined(BRO_IN_NETVAR) || ! defined(_home_hendra_Experiment_IAT_Analysis_build_analysis_bif)
#ifndef BRO_IN_NETVAR
#ifndef _home_hendra_Experiment_IAT_Analysis_build_analysis_bif
#define _home_hendra_Experiment_IAT_Analysis_build_analysis_bif
#include "bro-bif.h"
#endif
#endif

namespace BifEnum { namespace FeatureAnalysis { enum Analysis_ID {
	KS_ANALYSIS = 0,
	ENTROPY_ANALYSIS = 1,
	CCE_ANALYSIS = 2,
	NULL_ANALYSIS = 3,
	MULTIMODAL_ANALYSIS = 4,
	AUTOCORRELATION_ANALYSIS = 5,
}; } }
namespace BifType { namespace Enum{ namespace FeatureAnalysis {  extern EnumType * Analysis_ID;  } } }
namespace BifFunc { namespace FeatureAnalysis { extern Val* bro_Extract(Frame* frame, val_list*); }  }
namespace BifFunc { namespace FeatureAnalysis { extern Val* bro_Extract_vector(Frame* frame, val_list*); }  }
namespace BifFunc { namespace FeatureAnalysis { extern Val* bro_SetStepSize(Frame* frame, val_list*); }  }
namespace BifFunc { namespace FeatureAnalysis { extern Val* bro_RemoveConn(Frame* frame, val_list*); }  }
namespace BifFunc { namespace FeatureAnalysis { extern Val* bro_RegisterAnalysis(Frame* frame, val_list*); }  }
namespace BifFunc { namespace FeatureAnalysis { extern Val* bro_CalculateMetric(Frame* frame, val_list*); }  }
namespace FeatureAnalysis { extern EventHandlerPtr metric_event;  }
namespace BifEvent { namespace FeatureAnalysis {  void generate_metric_event(analyzer::Analyzer* analyzer, Val* id, Val* v);  }  } 

#endif
