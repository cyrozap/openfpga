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

#include <log.h>
#include <Greenpak4.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

Greenpak4ShiftRegister::Greenpak4ShiftRegister(
		Greenpak4Device* device,
		unsigned int matrix,
		unsigned int ibase,
		unsigned int oword,
		unsigned int cbase)
		: Greenpak4BitstreamEntity(device, matrix, ibase, oword, cbase)
		, m_clock(device->GetGround())
		, m_input(device->GetGround())
		, m_reset(device->GetGround())	//Hold shreg in reset if not used
		, m_delayA(1)					//default must be 0xf so that unused ones show as unused
		, m_delayB(1)
		, m_invertA(false)
{
}

Greenpak4ShiftRegister::~Greenpak4ShiftRegister()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Accessors

string Greenpak4ShiftRegister::GetDescription()
{
	char buf[128];
	snprintf(buf, sizeof(buf), "SHREG_%u", m_matrix);
	return string(buf);
}

vector<string> Greenpak4ShiftRegister::GetInputPorts() const
{
	vector<string> r;
	r.push_back("IN");
	r.push_back("nRST");
	r.push_back("CLK");
	return r;
}

void Greenpak4ShiftRegister::SetInput(string port, Greenpak4EntityOutput src)
{
	if(port == "IN")
		m_input = src;
	else if(port == "nRST")
		m_reset = src;
	else if(port == "CLK")
		m_clock = src;

	//ignore anything else silently (should not be possible since synthesis would error out)
}

vector<string> Greenpak4ShiftRegister::GetOutputPorts() const
{
	vector<string> r;
	r.push_back("OUTA");
	r.push_back("OUTB");
	return r;
}

unsigned int Greenpak4ShiftRegister::GetOutputNetNumber(string port)
{
	if(port == "OUTA")
		return m_outputBaseWord + 1;
	else if(port == "OUTB")
		return m_outputBaseWord;
	else
		return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization

bool Greenpak4ShiftRegister::CommitChanges()
{
	//Get our cell, or bail if we're unassigned
	auto ncell = dynamic_cast<Greenpak4NetlistCell*>(GetNetlistEntity());
	if(ncell == NULL)
		return true;

	if(ncell->HasParameter("OUTA_TAP"))
	{
		m_delayA = atoi(ncell->m_parameters["OUTA_TAP"].c_str());
		if( (m_delayA < 1) || (m_delayA > 16) )
		{
			LogError("Shift register OUTA_TAP must be in [1, 16]\n");
			return false;
		}
	}

	if(ncell->HasParameter("OUTB_TAP"))
	{
		m_delayB = atoi(ncell->m_parameters["OUTB_TAP"].c_str());
		if( (m_delayB < 1) || (m_delayB > 16) )
		{
			LogError("Shift register OUTB_TAP must be in [1, 16]\n");
			return false;
		}
	}

	if(ncell->HasParameter("OUTA_INVERT"))
		m_invertA = atoi(ncell->m_parameters["OUTA_INVERT"].c_str());

	return true;
}

bool Greenpak4ShiftRegister::Load(bool* /*bitstream*/)
{
	//TODO: Do our inputs
	LogError("Unimplemented\n");
	return false;
}

bool Greenpak4ShiftRegister::Save(bool* bitstream)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// INPUT BUS

	if(!WriteMatrixSelector(bitstream, m_inputBaseWord + 0, m_clock))
		return false;
	if(!WriteMatrixSelector(bitstream, m_inputBaseWord + 1, m_input))
		return false;
	if(!WriteMatrixSelector(bitstream, m_inputBaseWord + 2, m_reset))
		return false;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Configuration

	//Tap B comes first (considered output 0 in Silego docs but we flip so that A has the inverter)
	//Note that we use 0-based tap positions, while the parameter to the shreg is 1-based delay in clocks
	int delayB = m_delayB - 1;
	bitstream[m_configBase + 0] = (delayB & 1) ? true : false;
	bitstream[m_configBase + 1] = (delayB & 2) ? true : false;
	bitstream[m_configBase + 2] = (delayB & 4) ? true : false;
	bitstream[m_configBase + 3] = (delayB & 8) ? true : false;

	//then tap A
	int delayA = m_delayA - 1;
	bitstream[m_configBase + 4] = (delayA & 1) ? true : false;
	bitstream[m_configBase + 5] = (delayA & 2) ? true : false;
	bitstream[m_configBase + 6] = (delayA & 4) ? true : false;
	bitstream[m_configBase + 7] = (delayA & 8) ? true : false;

	//then invert flag
	bitstream[m_configBase + 8] = m_invertA;

	return true;
}
