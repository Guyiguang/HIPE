//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#pragma once
#include <coredata/NoneData.h>
#include <coredata/IODataType.h>

#include <coredata/ConnexData.h>
#include <corefilter/tools/filterMacros.h>
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <core/HipeStatus.h>
#include <corefilter/Model.h>
#include <corefilter/IFilter.h>
#include <corefilter/filter_export.h>

#include <string>
#include <data/FileImageData.h>
#include <data/VideoData.h>
#include <corefilter/datasource/DataSource.h>


namespace filter
{
	
	namespace datasource
	{
		class FILTER_EXPORT VideoDataSource : public IFilter, public DataSource
		{
			CONNECTOR(data::NoneData, data::Data);

			SET_NAMESPACE("vision/datasource")

			REGISTER(VideoDataSource, ()), _connexData(data::INDATA)
			{
				eSourceType = data::IODataType::VIDF;
				loop = false;
				atomic_state = false;
				a_isActive = false;
			}

			REGISTER_P(std::string, url);
		
			REGISTER_P(std::string, sourceType);

			REGISTER_P(bool, loop);

			data::IODataType eSourceType;

			VideoDataSource(const VideoDataSource &left)
			{
				this->video = left.video;
				this->a_isActive.exchange(left.a_isActive);
				this->atomic_state.exchange(left.atomic_state);
				this->url = left.url;
				this->sourceType = left.sourceType;
				this->loop = left.loop;
				this->eSourceType = left.eSourceType;
			}

			virtual data::IODataType getSourceType() const
			{
				return eSourceType;
			}

			void setSourceType(data::IODataType source_type)
			{
				eSourceType = source_type;
			}

			std::shared_ptr<data::IVideo> video;

			std::atomic<bool> atomic_state;

			std::atomic<bool> a_isActive;

			inline std::atomic<bool> & isActive() 
			{
				return a_isActive;
			}

			void setactive()
			{
				a_isActive.exchange(true);
			}

			HipeStatus process();

			/**
			 * \brief Be sure to call the dispose method before to destroy the object VideoDataSource
			 */
			virtual void dispose()
			{
				if (video)
				{
					video->closeFile();
					video.reset();
					a_isActive.exchange(false);
					atomic_state.exchange(false);
				}
				
			}

			HipeStatus intialize() override;
		};

		ADD_CLASS(VideoDataSource, url, sourceType, loop);
	}
}
