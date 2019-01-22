/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once
#include "HarmoniaErrorType.h"

class HarmoniaErrorData
{
public:
	HarmoniaErrorData(HarmoniaErrorType errorType);
	~HarmoniaErrorData();

	HarmoniaErrorType get_error_type();

private:
	HarmoniaErrorType _type;
};
