#pragma once
#include <data/IODataType.h>
#include <data/data_export.h>
#include <data/IOData.h>

namespace data {

	class IVideo
	{
	public:
		

		virtual void openFile() = 0;

		virtual Data newFrame() = 0;
	};

	/**
	 * \brief VideoData is the base data type used to handle videos.
	 * \tparam Derived
	 */
	template <typename Derived>
	class VideoData : public IOData<Data, Derived>, public IVideo
	{
	public:
		using IOData<Data, Derived>::IOData;


		VideoData(IODataType dataType) : IOData<Data, Derived>::IOData(dataType)
		{

		}

		VideoData(const VideoData &data) : IOData<Data, Derived>::IOData(data)
		{

		}
		virtual void openFile() { };

		virtual ~VideoData() {}

		void copyTo(VideoData& left) const
		{
			static_cast<const Derived&>(*this).copyTo(static_cast<Derived&>(left));
		}

		virtual Data newFrame()
		{
			return static_cast<Derived&>(*this).newFrame();
		}

		bool empty() const
		{
			return static_cast<const Derived&>(*this).empty();
		}
	};
}
