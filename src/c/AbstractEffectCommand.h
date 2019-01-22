/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once

class AbstractEffectCommand
{
public:
	AbstractEffectCommand();
	virtual ~AbstractEffectCommand();

	virtual void execute(int* combinedBuffer, int size);
	void set_enabled(bool enabled);
	bool get_enabled();

private:
	bool _enabled;
};

