/*

MIT License

Copyright(c) 2021 Hiroshi Teshima

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include  "opencv2/opencv.hpp"


#include <memory>
#include <vector>
#include <string>
#include <map>

#include <filesystem>	// C++17 standard header file name ,
						// VS2017 check project complier option

namespace fs = std::filesystem;



//-----------------------------------------------------------------------------
// internal use only
//-----------------------------------------------------------------------------

class UNRMaterialInfo;
typedef std::shared_ptr<UNRMaterialInfo> UNRMaterialInfoPtr;

typedef std::map<std::string, UNRMaterialInfoPtr> MaterialInfo;


class UNRMaterialInfo
{
public:
	UNRMaterialInfo() { m_pos =0; }
	~UNRMaterialInfo() { ; }

	UNRMaterialInfo(size_t pos)
	{
		m_pos = pos;

		m_x = 0;
		m_y = 0;
	}

	size_t		m_pos;			// obj line position
	fs::path	m_file_path;	// image file path

	cv::Mat		m_image;

	// composite map position
	int	m_x;
	int	m_y;
};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class UNRPlateauObj
{
public:
	UNRPlateauObj() { m_vt_offset = 0; }
	~UNRPlateauObj() { ; }


public:
static	bool	PackTextureRecursive(fs::path& obj_folder_path, fs::path& result_folder_path);
	bool	PackTextureSingleObj(fs::path& obj_file_path, fs::path& result_folder_path);

private:


	bool	LoadObj(fs::path& obj_file_path);
	bool	RecalcLayout();
	bool	SaveObj(fs::path& obj_folder_path);

	void	CreateNewTextureImage();
	void	ReaclcUV(UNRMaterialInfoPtr& info);


private:
	// file path
	fs::path	m_obj_path;
	fs::path	m_mtl_path;

	// obj file
	std::vector<std::string>	m_obj_line;
	std::string					m_coordinate_comment;

	// 'vt' top offset
	size_t	m_vt_offset;

	// material information
	MaterialInfo		m_obj_material;
	std::vector< UNRMaterialInfoPtr> m_info;

	// packed image size
	int		m_width;
	int		m_height;

	// texture image
	cv::Mat	m_new_texture;

};
