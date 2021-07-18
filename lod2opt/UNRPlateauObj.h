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

	std::string	m_material_name;

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
	UNRPlateauObj() {
		m_vt_offset = 0;
		m_vt_count = 0;
		m_append_vt_index = 0;
	}
	~UNRPlateauObj() { ; }


public:

	static	int	PackTextureRecursive(const fs::path& obj_folder_path, const fs::path& result_folder_path);
	int		PackTextureSingleObj(const fs::path& obj_file_path,const  fs::path& result_folder_path);


	bool	IsWavefrontObjFile(const fs::path& target_file);
private:


	bool	LoadObj(const fs::path& obj_file_path);
	bool	RecalcLayout();
	bool	SaveObj(const fs::path& obj_folder_path);

	void	CreateNewTextureImage();
	void	ReaclcUV(UNRMaterialInfoPtr& info);

	void	PrintUVFace(std::string& face_line, const char* insert_command = 0);
private:
	// file path
	fs::path	m_obj_path;
	fs::path	m_mtl_path;

	// obj file
	std::vector<std::string>	m_obj_line;
	std::string					m_coordinate_comment;

	size_t	m_vt_offset;	// 'vt' top offset
	size_t	m_f_offset;		// 'f' top offset

	// material information
	MaterialInfo		m_obj_material;
	std::vector< UNRMaterialInfoPtr> m_info;

	// packed image size
	int		m_width;
	int		m_height;

	// texture image
	cv::Mat	m_new_texture;


private:
	void	SetConvertedCheck(std::string& material, size_t pos);

	size_t	CheckConverted(std::string& material, size_t pos);
	void	ReplaceFcaeVt(size_t f_pos, std::map <size_t, size_t>& replace_vt);

	void	ShareNewVt();


	size_t		m_vt_count;
	size_t		m_append_vt_index;

	std::vector <std::string>		m_converted_vt_line;

	std::map<size_t, std::string>	m_check_mark_material;			// <original vt index><material name>
	std::map<size_t, size_t>		m_replace_face_vt_in_material;	// <vt index><vt index>
};
