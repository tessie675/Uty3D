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

//-----------------------------------------------------------------------------
// lod2opt
//
//  Merge material texture image chunk to one image file for wavefront obj 
//
// usage:
//
// lod2opt <obj_file> <result_folder>
//
//      obj_file : target obj file
//      result_folder : merge image file and .jpg .mtl .obj
//
//     result_folder ---+- hoe.obj
//				        +- hoge.mtl
//                      +- packed_image.jpg
//
//
//
// lod2opt  <obj_folder> <result_folder>
//
//      obj_folder : Recursively search folders and convert .obj files
//      result_folder : merge image file and .jpg .mtl .obj
//                      Same folder structure as the source
//
//     obj_folder -+- hoe.obj
//                 +- hoge.mtl
//                 +- texture --+- hoge1.jpg
//                 |            +- hoge2.jpg
//                 |
//                 + obj_folder1 -+- hoge1.obj
//                                +- hoge.mtl
//                                ....
//
//
//     result_folder --+- hoe.obj
//				       +- hoge.mtl
//                     +- packed_image.jpg
//                     |
//                     + obj_folder1 -+- hoge1.obj
//                                    +- hoge.mtl
//                                    ....
//
//
//-----------------------------------------------------------------------------

#include "UNRPlateauObj.h"

#include <clocale>
#include <iostream>

int main(int argc,char* argv[])
{
	std::setlocale(LC_ALL, "");


	if (argc != 3 )
	{
		std::cerr << "error : 2 args required" << std::endl;
		std::cerr << "lod2opt <obj_file> <result_folde>r" << std::endl;
		std::cerr << "or lod2opt <obj_folder> <result_folder>" << std::endl;
		return -1;
	}

	fs::path source_path(argv[1]);
	fs::path result_folder_path(argv[2]);

	if (!fs::exists(source_path))
	{
		std::cerr << "Error : not found target : " << source_path .string() << std::endl;
		return -2;
	}

	if (fs::is_directory(source_path))
	{
		return UNRPlateauObj::PackTextureRecursive(source_path, result_folder_path);
	}

	UNRPlateauObj wavefront_obj;
	if ( wavefront_obj.IsWavefrontObjFile(source_path))
	{
		return wavefront_obj.PackTextureSingleObj(source_path, result_folder_path);
	}

	return -1;
}

