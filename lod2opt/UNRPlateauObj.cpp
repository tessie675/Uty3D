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


#include "UNRPlateauObj.h"

#include "opencv2/imgcodecs.hpp"
#include <fstream>
#include <sstream>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//

bool UNRPlateauObj::PackTextureRecursive(fs::path & obj_folder_path, fs::path & result_folder_path)
{
	fs::path result_folder__base_path = result_folder_path;
	fs::path target_folder__base_path = obj_folder_path;

	if (!fs::exists(obj_folder_path))
	{
		// not exists input folder
		return false;
	}

	for (const auto & dir_entry : fs::directory_iterator(target_folder__base_path))
	{

		fs::path file_path = dir_entry.path();

		if (file_path.extension().string().compare(".obj") == 0)
		{
			if (!fs::exists(result_folder_path))
			{
				fs::create_directories(result_folder_path);
			}
			UNRPlateauObj wave_font_obj;
			wave_font_obj.PackTextureSingleObj(file_path, result_folder_path);
		}
		else if (dir_entry.is_directory())
		{
			fs::path dir_path = dir_entry.path();
			// copy file path
			fs::path new_result = result_folder__base_path;
			new_result /= dir_path.filename();

			PackTextureRecursive(dir_path, new_result);
		}

	}
	return true;
}


//-----------------------------------------------------------------------------
bool UNRPlateauObj::PackTextureSingleObj(fs::path & obj_file_path, fs::path & result_folder_path)
{
	LoadObj(obj_file_path);
	RecalcLayout();
	SaveObj(result_folder_path);

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// read wavefront obj
//

bool UNRPlateauObj::LoadObj(fs::path& obj_file_path)
{
	//-------------OBJ -----------
	//
	m_vt_offset = 0;
	std::ifstream ifs;

	ifs.open(obj_file_path, std::ios_base::in);
	if (!ifs)
	{
		return false;
	}

	fs::path file_name = obj_file_path.filename();

	m_obj_path = obj_file_path;

	size_t	line_count = 0;
	for (;ifs;)
	{
		std::string obj_line;
		std::getline(ifs, obj_line);
		if (ifs.eof())
		{
			// end of file
			break;
		}
		if (obj_line[0] == '#') {
			if (obj_line.find("COORDINATE_SYSTEM") != std::string::npos)
			{
				m_coordinate_comment = obj_line;
			}
			continue;		// skip comment
		}

		m_obj_line.emplace_back(obj_line);

		// check 'vt'
		if (obj_line[0] == 'v' && obj_line[1] == 't')
		{
			if (m_vt_offset == 0)
			{
				m_vt_offset = line_count;
			}
		} else if (obj_line[0] == 'u' || obj_line[0] == 'm')
		{
			// check usemtl
			std::istringstream iss(obj_line);
			std::string token;

			std::getline(iss, token, ' ');
			if (token.compare("usemtl") == 0)
			{
				// get material name
				std::getline(iss, token, ' ');		
				m_obj_material[token] = m_info.emplace_back(std::make_shared<UNRMaterialInfo>(line_count));
			}
			else if (token.compare("mtllib") == 0)
			{
				// get mtl path name
				std::getline(iss, token, ' ');
				m_mtl_path = token;
			}
		}
		line_count++;
	}
	if (!ifs.eof())
	{
		return false;
	}

	//-------------MTL -----------
	// load mtl file
	fs::path mtl_abs_path;
	if (m_mtl_path.is_relative())
	{
		mtl_abs_path = m_obj_path.parent_path();
		mtl_abs_path.append(m_mtl_path.c_str());

	}
	else {
		mtl_abs_path = m_mtl_path;
	}


	std::ifstream ifs_mtl;
	ifs_mtl.open(mtl_abs_path, std::ios_base::in);

	std::string newmtl_tag;
	std::string texture_file_path;
	for (;ifs_mtl;)
	{
		std::string mtl_line;
		std::getline(ifs_mtl, mtl_line);
		if (ifs_mtl.eof())
		{
			// end of file
			break;
		}
		if (mtl_line[0] == 'n'|| mtl_line[0] == 'm')
		{
			// check usemtl
			std::istringstream iss(mtl_line);
			std::string token;

			std::getline(iss, token, ' ');
			if (token.compare("newmtl") == 0)
			{
				std::getline(iss, token, ' ');
				if ( !newmtl_tag.empty())
				{
					m_obj_material[newmtl_tag]->m_file_path = texture_file_path;
				}
				newmtl_tag = token;
			}
			else if (token.compare("map_Kd") == 0)
			{
				std::getline(iss, token, ' ');
				texture_file_path = token;
			}

		}
	}
	// last material
	m_obj_material[newmtl_tag]->m_file_path = texture_file_path;


	return true;

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool UNRPlateauObj::RecalcLayout()
{
	// get image information
	int area = 0;
	for (auto i = m_obj_material.begin(); i != m_obj_material.end(); ++i)
	{
		fs::path texture_path = i->second->m_file_path;
		if (!texture_path.is_absolute())
		{
			//
			fs::path texture_abs = m_obj_path.parent_path();
			texture_abs /= texture_path;
			texture_path = texture_abs.lexically_normal();
		}
		std::wstring wpath = texture_path.c_str();
		std::string mb_path(wpath.begin(), wpath.end());

		i->second->m_image = cv::imread(mb_path.c_str());
		int row =  i->second->m_image.rows;
		int col = i->second->m_image.cols;

		area += (row*col);

	}

	// descending sort by rows 
	sort(m_info.begin(), m_info.end(), [](auto const& lhs, auto const& rhs) {
		if (lhs->m_image.rows == rhs->m_image.rows) return lhs->m_image.cols > rhs->m_image.cols;
		return lhs->m_image.rows > rhs->m_image.rows;
	});

	//
	int size = (int)sqrt(area) * 1.1;
	
	int	next_x = -1;
	int	cur_y = 0;
	int	next_y = 0;

	size_t idx;
	for (idx=0;idx<m_info.size();idx++)
	{
		if (next_x == -1)
		{
			next_x = m_info[idx]->m_image.cols;
			next_y = m_info[idx]->m_image.rows;
		}
		else {
			if ((next_x + m_info[idx]->m_image.cols) > size)
			{
				cur_y = cur_y + next_y;

				next_x = m_info[idx]->m_image.cols;
				next_y = m_info[idx]->m_image.rows;

				m_info[idx]->m_x = 0;
				m_info[idx]->m_y = cur_y;
			}
			else {
				m_info[idx]->m_x = next_x;
				m_info[idx]->m_y = cur_y;

				next_x += m_info[idx]->m_image.cols;
			}
		}
	}

	m_width = size;
	m_height = cur_y + next_y;

	// remap UV coordinate
	for (idx = 0; idx < m_info.size(); idx++)
	{
		ReaclcUV(m_info[idx]);
	}

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void UNRPlateauObj::ReaclcUV(UNRMaterialInfoPtr& info)
{
	double wx = (double)m_width;
	double wy = (double)m_height;

	double w_orgine_x = (double)info->m_x;
	double w_orgine_y = (double)info->m_y;

	double w_width = (double)info->m_image.cols;
	double w_height = (double)info->m_image.rows;


	typedef std::map<size_t, size_t>	CheckMark;
	CheckMark uv_converted;

	size_t start = info->m_pos+1;

	size_t idx;
	for (idx = start; m_obj_line[idx][0] != 'u'; idx++)
	{
		std::istringstream iss(m_obj_line[idx]);
		std::string token;

		std::getline(iss, token, ' ');	// skip 'f'

		int j;
		for (j = 0; !iss.eof(); j++)
		{
			std::getline(iss, token, ' ');	//

			std::istringstream coord(token);
			std::string vertex;

			std::getline(coord, vertex, '/');	// skip geomrtry vertex
			std::getline(coord, vertex, '/');	// uv

			if (vertex.empty())
			{
				// error
				continue;
			}
			size_t vpos = atoi(vertex.c_str());
			if (vpos == 1)
			{
				int i = 0;
			}
			vpos += m_vt_offset - 1;


			if (uv_converted.find(vpos) != uv_converted.end()) {
				continue;
			}



			std::istringstream vt(m_obj_line[vpos]);
			std::string uv_coord;

			std::getline(vt, uv_coord, ' ');	// skip 'vt'
			if (uv_coord == "v")
			{
				continue;
			}

			std::getline(vt, uv_coord, ' ');	// u
			double u = atof(uv_coord.c_str());

			double image_x = w_width * u + w_orgine_x;
			double new_u = image_x / wx;


			std::getline(vt, uv_coord, ' ');	// v
			double v = atof(uv_coord.c_str());

			double image_y = (w_height - w_height * v ) + w_orgine_y;
			double new_v = (wy - image_y) / wy;

			char uv_buff[100];
			sprintf_s(uv_buff, 100, "vt %.6lf %.6lf", new_u, new_v);

			m_obj_line[vpos] = uv_buff;

			uv_converted[vpos] = 1;


		}
		if (idx + 1 == m_obj_line.size()) { break; }

	}

	return;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void UNRPlateauObj::CreateNewTextureImage()
{
	m_new_texture = cv::Mat(m_height, m_width, CV_8UC3);

	size_t idx;
	for (idx = 0; idx < m_info.size(); idx++)
	{

		cv::Mat roi_dest(m_new_texture, cv::Rect(m_info[idx]->m_x, m_info[idx]->m_y,
			m_info[idx]->m_image.cols, m_info[idx]->m_image.rows));
		m_info[idx]->m_image.copyTo(roi_dest);

	}

	/***
	int new_size = 1024;
	cv::Mat resizeTexture(new_size, new_size, CV_8UC3);
	cv::resize(m_new_texture, resizeTexture, resizeTexture.size());

	cv::imshow("new_texture", resizeTexture);
	cv::waitKey();
	***/


	return;
}

//-----------------------------------------------------------------------------

bool UNRPlateauObj::SaveObj(fs::path& obj_folder_path)
{
	// check result folder
	if (!fs::exists(obj_folder_path))
	{
		fs::create_directory(obj_folder_path);
	}

	CreateNewTextureImage();

	fs::path new_obj_path = obj_folder_path;
		
	fs::path obj_file_name =  m_obj_path.filename();
	new_obj_path /= obj_file_name;

	fs::path mtl_name = obj_file_name.stem();
	std::wstring wmtl_name = mtl_name.c_str();
	std::string mb_mtl(wmtl_name.begin(), wmtl_name.end());

	std::ofstream of_obj;
	of_obj.open(new_obj_path, std::ios_base::out);

	if (!of_obj)
	{
		return false;
	}

	// comment
	if (!m_coordinate_comment.empty()) { of_obj << m_coordinate_comment << std::endl; }
	of_obj << "# texture packed obj" << std::endl;
	of_obj << "# " << std::endl;

	// write obj
	bool out_usemtl = false;
	size_t i;
	for (i = 0; i < m_obj_line.size(); i++)
	{
		if (m_obj_line[i][0] == 'u')
		{
			if (m_obj_line[i].find("usemtl") != std::string::npos)
			{
				if (!out_usemtl)
				{
					of_obj << "usemtl " << mb_mtl << std::endl;
					out_usemtl = true;
				}
				continue;
			}
		}
		else if (m_obj_line[i][0] == 'm')
		{
			if (m_obj_line[i].find("mtllib") != std::string::npos)
			{
				of_obj << "mtllib " << mb_mtl << ".mtl" << std::endl;

				continue;
			}

		}
		of_obj << m_obj_line[i] << std::endl;
	}

	// texture image file
	fs::path texture_abs_path = obj_folder_path;
	texture_abs_path /= obj_file_name.replace_extension(".jpg");
	
	std::wstring w_texture_path = texture_abs_path.c_str();
	std::string mb_texture_path(w_texture_path.begin(), w_texture_path.end());

	int max_len = m_new_texture.rows;
	if (m_new_texture.rows < m_new_texture.cols)
	{
		max_len = m_new_texture.cols;
	}

	// image size 512 --> 16K(max)
	int new_size = 512;
	if (max_len > 16384)    { new_size = 16384; }
	else if (max_len > 8192) { new_size = 8192; }
	else if (max_len > 4096) { new_size = 4096;	}
	else if (max_len > 2048) { new_size = 2048; }
	else if (max_len > 1024) { new_size = 1024; }


	cv::Mat resizeTexture(new_size, new_size, CV_8UC3);
	cv::resize(m_new_texture, resizeTexture, resizeTexture.size());

	std::vector<int> paramas;
	paramas.emplace_back(cv::ImwriteFlags::IMWRITE_JPEG_QUALITY);
	paramas.emplace_back(50);

	cv::imwrite(mb_texture_path, resizeTexture, paramas);

	std::wstring w_texture_name= obj_file_name.replace_extension(".jpg");
	std::string mb_texture_file_name(w_texture_name.begin(), w_texture_name.end());

	// mtl
	fs::path new_mtl_path = obj_folder_path;
	new_mtl_path /= obj_file_name.replace_extension(".mtl");

	std::ofstream of_mtl;
	of_mtl.open(new_mtl_path, std::ios_base::out);

	of_mtl << "# texture packed obj" << std::endl;
	of_mtl << "newmtl " << mb_mtl << std::endl;
	of_mtl << "Kd 1.0 1.0 1.0 " << std::endl;
	of_mtl << "map_Kd " << mb_texture_file_name << std::endl;



	return true;

}
