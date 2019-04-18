#ifndef __FILE_SEARCH_H__
#define __FILE_SEARCH_H__

#include <string>
#include <vector>
#include <Windows.h>


class FileSearch{
public:
	FileSearch(const std::string dir)
		:_dir(dir),
		_filterType(DIR | FILE),
		_isSearchSubdir(false){
	}

	enum FileType{
		DIR = 0x01,
		FILE = 0x02,
	};

	struct FileInfo{
		FileType type;
		std::string fileName;
		std::string filePath;
		std::string suffix;
	};


	typedef std::vector<FileInfo> FileInfoList;

	/*
	* Set Filter File Type.
	* @param type defualt is FT_DIR | FT_FILE.
	*/
	void SetFilterType(int type){
		_filterType = type;
	}

	/*
	* Set Filter Suffix.
	* @param suffix default is empty, not set filter suffix.
	*/
	void SetFilterSuffix(const std::string& suffix){
		_filterSuffix = suffix;
	}

	/*
	* Set If Search Sub Directory.
	* @isSearchSubdir If is search sub directory, Default is false.
	*/
	void SetSearchSubDir(bool isSearchSubdir){
		_isSearchSubdir = isSearchSubdir;
	}

	/*
	* Get File Info List.
	* @return File info list.
	*/
	FileInfoList GetFileInfoList(){
		FileInfoList infos;
		search(_dir, infos);
		return infos;
	}

private:
	void search(const std::string& dir, FileInfoList& infos){
		WIN32_FIND_DATAA FindFileData;

		std::string searchDir;
		searchDir = dir + "\\*";
		HANDLE hFind = ::FindFirstFileA(searchDir.c_str(), &FindFileData);

		if (hFind == INVALID_HANDLE_VALUE){
			return;
		}

		while (TRUE){
			DWORD attr = FindFileData.dwFileAttributes;

			do
			{
				if (attr & FILE_ATTRIBUTE_SYSTEM || attr & FILE_ATTRIBUTE_HIDDEN)
					break;
				if (!strcmp(FindFileData.cFileName, ".") || !strcmp(FindFileData.cFileName, ".."))
					break;

				FileInfo info;
				info.fileName = FindFileData.cFileName;
				info.filePath = dir + "\\";
				info.filePath += info.fileName;

				if (attr & FILE_ATTRIBUTE_DIRECTORY){ //dir
					if (_filterType & DIR){
						info.type = DIR;

						infos.push_back(info);
					}

					if (_isSearchSubdir){
						search(info.filePath, infos);
					}
				}
				else{//file
					if (_filterType & FILE){
						info.type = FILE;
						int sindex = info.fileName.find_last_of('.');
						if (sindex != -1)
							info.suffix = info.fileName.substr(sindex + 1);

						bool add(true);
						if (!_filterSuffix.empty()){
							if (_filterSuffix != info.suffix){
								add = false;
							}
						}

						if (add)
							infos.push_back(info);
					}
				}
			} while (0);

			if (!FindNextFileA(hFind, &FindFileData))
				break;
		}
		FindClose(hFind);
	}
private:
	std::string _dir;
	int _filterType;
	std::string _filterSuffix;
	bool _isSearchSubdir;
};


#endif //!__FILE_SEARCH_H__