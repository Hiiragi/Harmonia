/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once

#include "HarmoniaErrorData.h"
#include "HarmoniaErrorType.h"

HarmoniaErrorData::HarmoniaErrorData(HarmoniaErrorType type)
{
	_type = type;
}

HarmoniaErrorData::~HarmoniaErrorData()
{

}

HarmoniaErrorType HarmoniaErrorData::get_error_type()
{
	return _type;
}
