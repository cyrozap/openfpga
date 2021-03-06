/***********************************************************************************************************************
 * Copyright (C) 2016 Andrew Zonenberg and contributors                                                                *
 *                                                                                                                     *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General   *
 * Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) *
 * any later version.                                                                                                  *
 *                                                                                                                     *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied  *
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for     *
 * more details.                                                                                                       *
 *                                                                                                                     *
 * You should have received a copy of the GNU Lesser General Public License along with this program; if not, you may   *
 * find one here:                                                                                                      *
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt                                                              *
 * or you may search the http://www.gnu.org website for the version 2.1 license, or you may write to the Free Software *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA                                      *
 **********************************************************************************************************************/

#ifndef Greenpak4RCOscillator_h
#define Greenpak4RCOscillator_h

class Greenpak4PowerRail;

/**
	@brief The RC (2 MHz / 25 kHz) oscillator
 */
class Greenpak4RCOscillator : public Greenpak4BitstreamEntity
{
public:

	//Construction / destruction
	Greenpak4RCOscillator(
		Greenpak4Device* device,
		unsigned int matrix,
		unsigned int ibase,
		unsigned int oword,
		unsigned int cbase);
	virtual ~Greenpak4RCOscillator();

	//Serialization
	virtual bool Load(bool* bitstream);
	virtual bool Save(bool* bitstream);

	virtual std::string GetDescription();

	//Get the power-down input (used for DRC)
	Greenpak4EntityOutput GetPowerDown()
	{ return m_powerDown; }

	//Enable accessors

	bool GetPowerDownEn()
	{ return m_powerDownEn; }

	bool IsConstantPowerDown();

	virtual void SetInput(std::string port, Greenpak4EntityOutput src);
	virtual unsigned int GetOutputNetNumber(std::string port);

	virtual std::vector<std::string> GetInputPorts() const;
	virtual std::vector<std::string> GetOutputPorts() const;

	virtual bool CommitChanges();

protected:

	///Power-down input (if implemented)
	Greenpak4EntityOutput m_powerDown;

	///Power-down enable
	bool m_powerDownEn;

	///Auto power-down
	bool m_autoPowerDown;

	///Output pre-divider
	int m_preDiv;

	///Output post-divider
	int m_postDiv;

	///Clock frequency (true = 2 MHz, false = 25 kHz)
	bool m_fastClock;
};

#endif
