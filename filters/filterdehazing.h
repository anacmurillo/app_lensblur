#pragma once

#include "filter.h"

class FilterDehazing {
	static cv::Mat dehaze(const cv::Mat& image, const cv::Mat& transmittance, double min, double max)
	{
	    std::chrono::time_point<std::chrono::system_clock> start;

	    start = std::chrono::system_clock::now();
	    
	    cv::Mat colored_transmittance;
	    cv::normalize(transmittance, colored_transmittance, min*255.0, max*255.0, cv::NORM_MINMAX, -1);
	    cv::Mat1b atmosphere_mask  = (colored_transmittance <= 255.0*(min + 0.1*(max-min)));
	    cv::Scalar atmosphere = cv::mean(image, atmosphere_mask);
	    atmosphere/=255.0f;
	    
	    cv::cvtColor(colored_transmittance, colored_transmittance, CV_GRAY2RGB);
	    cv::Mat num;
	    cv::multiply(atmosphere, (colored_transmittance*(-1.0) + cv::Scalar(255.0,255.0,255.0)), num  , 1, CV_32FC3);
	    cv::Mat imagef;
	    image.convertTo(imagef, CV_32FC3);
	    cv::subtract(imagef, num, num);
	    cv::max(num, cv::Scalar(0.0,0.0,0.0), num);
	    cv::Mat sol;
	    cv::divide(num, colored_transmittance, sol, 255.0f, CV_8UC3);

	    return sol;
	}

public:
	std::vector<std::string> propagatedValues() const
       	{    
		return std::vector<std::string>{{
			std::string("Transmittance")
		}};    
	
	}

	virtustd::vector<std::tuple<std::string, float, float>> floatValues() const
       	{    
		return std::vector<std::tuple<std::string, float, float>>{{
			std::make_tuple(std::string("Effect"),0.0f,1.0f)
		}};    
	}

	cv::Mat apply(const cv::Mat& input_image, 
			const std::vector<std::shared_ptr<cv::Mat>>& propagated_values,
			const std::vector<float>& float_values) const
	{	
		auto  transmittance = propagated_values[0];
		float intensity     = float_values[0];
	        double min, max;
		cv::minMaxLoc(*transmittance, &min, &max);
		
		double min_t = ((intensity/100.0)*min) + ((1.0 - (intensity/100.0))*max);//0.05;
		double max_t = max;//0.95;
        
        	return dehaze(input_image, *transmittance, min_t, max_t);
	}

}
