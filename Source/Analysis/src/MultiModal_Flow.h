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
* MultiModal_Flow.h : analysis engine to calculate multi modal estimation based *
*       on the formula sum((n_i)^2)/(max(n_i))^2.                               *
* Based on the technical report TR-2015-02-01 Communication Networks Group at   *
*       the Institute of Telecommunications Vienna University of Technology.    *
*       F. Iglesias, R. Annessi, D. Frkat, V. Bernhardt, M.Nitzschke, T.Zseby.  *
*       Preparation of the Network Security Advanced Lab: Detection of Covert   *
*       Channels                                                                *
\*******************************************************************************/

#ifndef AUX_PLUGINS_MULTIMODAL_FLOW_H
#define AUX_PLUGINS_MULTIMODAL_FLOW_H

#include <vector>                               // vector
#include <memory>                               // shared_ptr
#include "Analysis.h"                           // superclass
#include "Histogram.h"                          // Histogram

namespace CCD {

// Flow analyzer which uses a multimodal analysis on the possible symbols on
// the histogram
class MultiModal : public FeatureAnalyzer {
public:
	MultiModal(std::shared_ptr<Histogram> data) : FeatureAnalyzer(data) {}
	virtual ~MultiModal() {};

	virtual double calculate_metric();
};

}

#endif
