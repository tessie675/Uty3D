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
// args
//
//
// lod2opt [-r] obj_folder result_folder
//
//     obj_folder -+- hoe.obj
//                 +- hoge.mtl
//                 +- texture --+- hoge1.jpg
//                              +- hoge2.jpg
//
//
//     result_folder --- obj_folder -+- hoe.obj
//				                     +- hoge.mtl
//                                   +- packed_image.jpg
//
//     -r  recursive traverse folder
//

#include "UNRPlateauObj.h"

#include <clocale>
#include <iostream>

int main(int argc,char* argv[])
{
	std::setlocale(LC_ALL, "");

	fs::path obj_folder_path;
	fs::path result_folder_path;
	UNRPlateauObj wavefront_obj;

	if (argc == 4 )
	{
		if (argv[1][0] && argv[1][1] && argv[1][0] == '-' && argv[1][1] == 'r')
		{
			obj_folder_path = argv[2];
			result_folder_path = argv[3];

			UNRPlateauObj::PackTextureRecursive(obj_folder_path, result_folder_path);

			return 0;
		} else 
		{
			std::cerr << "error : 3 args -r option required" << std::endl;
			std::cerr << "lod2opt obj_folder result_folder" << std::endl;
			return -1;
		}

	}
	else if (argc != 3)
	{
		std::cerr << "error : 2 args" << std::endl;
		std::cerr << "　lod2opt obj_folder result_folder" << std::endl;
		return -1;
	}

	obj_folder_path = argv[1];
	result_folder_path = argv[2];

	wavefront_obj.PackTextureSingleObj(obj_folder_path, result_folder_path);



	return 0;
}

