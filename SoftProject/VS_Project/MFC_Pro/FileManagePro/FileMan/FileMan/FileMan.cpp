// FileMan.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <direct.h>
#include <io.h> 
#include <fstream>

using namespace std;

bool is_folder(string file_name)
{
    if (file_name.empty()) {
        cout << "is_folder err !!! " << endl;
        return false;
    }

    struct _stat buf;

    _stat(file_name.c_str(), &buf);
    // �ж����ļ��л����ļ�
    if (_S_IFDIR & buf.st_mode) {
        return true;
    } else {
        return false;
    }
}

vector<string> scan_dir(string path)
{
    long hFile = 0;
    struct _finddata_t fileInfo;
    vector<string> folder_1;//һ���ļ����������ļ�(��)��

    if (path.empty()) {
        cout << "scan_dir err !!! " << endl;
        return folder_1;
    }

    cout << "scan_dir " << path << endl;

    if (path.substr(path.length() - 1) != "/") {
        path += "/";
    }

    hFile = _findfirst(path.append("*").c_str(), &fileInfo);//��·��ĩβ���ͨ���
    if (hFile == -1) {
        exit(0);
    }
    folder_1.push_back(fileInfo.name);
    while (_findnext(hFile, &fileInfo) == 0) {    //����ҵ���һ�����У��Ǿͼ���
        folder_1.push_back(fileInfo.name);
    }

    _findclose(hFile);

    return folder_1;
}

//ɾ���ļ��У�����string�ַ������͵����·��
void folder_remove(string dirPath)  
{
    vector<string>::iterator ite;
    vector<string> dir_file_list;

    if (dirPath.empty()) {
        cout << "folder_remove err !!! " << endl;
        return;
    }

    if (_access(dirPath.c_str(), 0) == -1) {
        cout << dirPath << " ������" << endl;
        return;
    }
    
    string dir_path_tmp = dirPath + "/";

    dir_file_list = scan_dir(dir_path_tmp);

    for (ite = dir_file_list.begin(); ite != dir_file_list.end(); ite++) {
        if ((*ite == ".") || (*ite == "..")) {
            continue;
        }
        string dir_tmp = dir_path_tmp + *ite;

        if (is_folder(dir_tmp)) {
            folder_remove(dir_tmp);
        }
        else {
            DeleteFile(dir_tmp.c_str()); // ɾ���ļ�
            cout << "remove " << dir_tmp << endl;
        }
    }
    RemoveDirectory(dirPath.c_str()); // ɾ���ļ��У����ļ��в������������ɾ��
    cout << "remove " << dirPath << endl;
}

//�����ļ��е��µ����ļ���ͬ���������·��
void folder_copy(string ori_folder, string dest_folder)
{
    if ((ori_folder.empty()) || (dest_folder.empty())) {
        cout << "folder_copy err !!! " << endl;
        return;
    }

    // ����ļ��Ƿ���ڣ��������򴴽�
    if (_access(dest_folder.c_str(), 0) == -1) {
        _mkdir(dest_folder.c_str());        // ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��
    }
    ori_folder  += "/";
    dest_folder += "/";

    vector<string> dir_file_list;
    dir_file_list = scan_dir(ori_folder);
    vector<string>::iterator ite;

    for (ite = dir_file_list.begin(); ite != dir_file_list.end(); ite++) {
        if ((*ite == ".") || (*ite == "..")) {
            continue;
        }
        string oriFileName  = ori_folder + *ite;
        string destFileName = dest_folder + *ite;

        if (is_folder(oriFileName)) {
            // ����ļ��Ƿ���ڣ��������򴴽�
            if (_access(destFileName.c_str(), 0) == -1) {
                _mkdir(destFileName.c_str());        // ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��
                cout << "mkdir " << destFileName << endl;
            }
            oriFileName  += "/";
            destFileName += "/";
            folder_copy(oriFileName, destFileName);
        }
        else {
            CopyFile(oriFileName.c_str(), destFileName.c_str(), false);
            cout << "copy " << oriFileName << " to " << destFileName << endl;
        }
    }
}

void folder_find(string find_name, string find_path, string sour_folder, string oper_mode = "FIND")
{
    if ((find_name.empty()) || (find_path.empty()) || (oper_mode.empty()) || (_access(find_path.c_str(), 0) == -1)) {
        cout << "folder_find err !!! " << endl;
        return;
    }

    if (find_path.substr(find_path.length() - 1) != "/") {
        find_path += "/";
    }
    vector<string> folder_name1 = scan_dir(find_path);   //���1��Ŀ¼�ļ�(��)��
    vector<string>::iterator ite;

    for (ite = folder_name1.begin(); ite != folder_name1.end(); ite++) {
        string path_tmp1 = find_path + *ite;

        if ((*ite == ".") || (*ite == "..") || (!is_folder(path_tmp1))) {
            continue;
        }

        if (*ite == find_name) {
            string path_tmp = find_path;
            if (oper_mode == "COPY") {  
                if (sour_folder.empty()) {
                    cout << "folder_find COPY err !!! " << endl;
                    return;
                }

                path_tmp = path_tmp + sour_folder.substr(sour_folder.find_last_of('/') + 1);   // ƴ���ļ�����·��
                folder_copy(sour_folder, path_tmp);     // �����ļ���

            } else if (oper_mode == "REMOVE") {
                path_tmp = find_path + *ite;
                folder_remove(path_tmp);

            } else if (oper_mode == "REPLACE") {
                if (sour_folder.empty()) {
                    cout << "folder_find REPLACE err !!! " << endl;
                    return;
                }

                path_tmp = find_path + *ite;
                folder_remove(path_tmp);
                path_tmp = find_path + sour_folder.substr(sour_folder.find_last_of('/') + 1);   // ƴ���ļ�����·��
                folder_copy(sour_folder, path_tmp);     // �����ļ���

            } else {
                path_tmp = find_path + *ite;
            }
            cout << path_tmp << endl;
        }
        else {
            path_tmp1 += "/";
            folder_find(find_name, path_tmp1, sour_folder, oper_mode);
        }
    }
}

vector<string> cmd_parse(string cmd_str)
{
    string str_tmp;
    vector<string> cmd_str_buf;
    int pos = 0;

    cmd_str_buf.clear();

    string str = cmd_str.substr(0, 2);
    if ((cmd_str.substr(0, 2) == "//") || (cmd_str.length() < 1)) {
        return cmd_str_buf;
    }

    pos = cmd_str.find_first_of(");");
    if (pos > 0) {
        char _c = cmd_str.c_str()[pos-1];
        for (unsigned int i = pos-1; i<cmd_str.length(); i--) {
            if (cmd_str.c_str()[i] != ' ') {
                break;
            }
            pos--;
        }
        cmd_str = cmd_str.substr(0, pos);
    }

    pos = cmd_str.find_first_of("(");
    if (pos > 0) {
        unsigned int por_sta = 0;
        for(por_sta = 0; por_sta<cmd_str.length(); por_sta++) {
            if (cmd_str.c_str()[por_sta] != ' ') {
                break;
            }
        }
        cmd_str_buf.push_back(cmd_str.substr(por_sta, pos- por_sta));
        cmd_str = cmd_str.substr(pos + 1, cmd_str.length());
    }

    while (1) {
        pos = cmd_str.find_first_of(",");
        if (pos > 0) {
            unsigned int por_sta = 0;
            for (por_sta = 0; por_sta<cmd_str.length(); por_sta++) {
                if (cmd_str.c_str()[por_sta] != ' ') {
                    break;
                }
            }
            cmd_str_buf.push_back(cmd_str.substr(por_sta, pos - por_sta));
            cmd_str = cmd_str.substr(pos + 1, cmd_str.length());
        }
        else {
            unsigned int por_sta = 0;
            for (por_sta = 0; por_sta<cmd_str.length(); por_sta++) {
                if (cmd_str.c_str()[por_sta] != ' ') {
                    break;
                }
            }
            cmd_str_buf.push_back(cmd_str.substr(por_sta, cmd_str.length() - por_sta));
            break;
        }
    }

    return cmd_str_buf;
}

// �����ļ�
void copy_file(string ori_file, string dest_file)
{
    if ((ori_file.empty()) || (dest_file.empty())) {
        return;
    }

    CopyFile(ori_file.c_str(), dest_file.c_str(), false);
    cout << "copy " << ori_file << " to " << dest_file << endl;
}

// ɾ���ļ�
void remove_file(string file_path)
{
    if (file_path.empty()) {
        return;
    }
    DeleteFile(file_path.c_str()); 
    cout << "remove " << file_path << endl;
}

// �滻�ļ����ݣ���old_str�滻Ϊnew_st��new_strΪ��ʱ�������ļ������к���old_str����
void file_str_replace(string file_path, string old_str, string new_str)
{
    if ((old_str == new_str) || (old_str.empty()) || (file_path.empty())) {
        return;
    }

    string line_str;
    ifstream file_in(file_path);
    fstream file_out;
    file_out.open("./_temp_file.txt", ios::out);
    if (!file_out) {
        return;
    }

    while (getline(file_in, line_str)) {
        if (line_str.find(old_str) != string::npos) {
            if (new_str.empty()) {
                continue;
            }
            while (1) {
                line_str == line_str.replace(line_str.find(old_str), old_str.length(), new_str);
                if (line_str.find(old_str) == string::npos) {
                    break;
                }
            }
        }
        file_out << line_str << endl;
    }

    file_out.close();

    remove_file(file_path);
    copy_file("./_temp_file.txt", file_path);
    remove_file("./_temp_file.txt");
}

//  �滻�ļ����µ�����file_type�����ļ����ݣ���old_str�滻Ϊnew_str�� new_strΪ��ʱ�������ļ������к���old_str����
void folder_str_replace(string file_path, string file_type, string old_str, string new_str)
{
    vector<string>::iterator ite;
    vector<string> dir_file_list;

    if ((old_str == new_str) || (old_str.empty()) || (file_path.empty())) {
        return;
    }

    if (_access(file_path.c_str(), 0) == -1) {
        cout << file_path << " ������" << endl;
        return;
    }

    string dir_path_tmp = file_path + "/";

    dir_file_list = scan_dir(dir_path_tmp);

    for (ite = dir_file_list.begin(); ite != dir_file_list.end(); ite++) {
        if ((*ite == ".") || (*ite == "..")) {
            continue;
        }
        string dir_tmp = dir_path_tmp + *ite;
        string suffix_str = "." + dir_tmp.substr(dir_tmp.find_last_of('.') + 1);  //��ȡ�ļ�����

        if (is_folder(dir_tmp)) {
            folder_str_replace(dir_tmp, file_type, old_str, new_str);
        }
        else {
            if (suffix_str == file_type) {
                file_str_replace(dir_tmp, old_str, new_str);
                cout << "file_str_replace " << dir_tmp << endl;
            }
        }
    }

    cout << "folder_str_replace " << file_path << endl;
}

// �ļ���������ɾ������ʼɾ�������к���start_str  ����ɾ�������к���end_str
void file_section_remove(string file_path, string start_str, string end_str)
{
    if ((start_str.empty()) || (file_path.empty())) {
        return;
    }

    bool section_remove_flag = false;
    string line_str;
    ifstream file_in(file_path);
    fstream file_out;
    file_out.open("./_temp_file.txt", ios::out);
    if (!file_out) {
        return;
    }

    while (getline(file_in, line_str)) {
        if (section_remove_flag) {
            if (line_str.find(end_str) != string::npos) {
                section_remove_flag = false;
            }
        } else {
            if (line_str.find(start_str) != string::npos) {
                section_remove_flag = true;
            } else {
                file_out << line_str << endl;
            }
        }        
    }

    file_out.close();

    remove_file(file_path);
    copy_file("./_temp_file.txt", file_path);
    remove_file("./_temp_file.txt");
}

int main()
{
//    folder_find("Project", "C:/Users/12436/Desktop/1234/Examples", "", "REMOVE");                                         // ɾ��
//    folder_find("User", "C:/Users/12436/Desktop/1234/Examples", "C:/Users/12436/Desktop/1234/Libraries", "COPY");         // ����
//    folder_find("Libraries", "C:/Users/12436/Desktop/1234/Examples", "C:/Users/12436/Desktop/1234/Libraries", "REPLACE"); // �滻
//    copy_file("C:/Users/12436/Desktop/1234/Libraries/Source/debug.c", "C:/Users/12436/Desktop/1234/debug.c");
//    remove_file("C:/Users/12436/Desktop/1234/debug.c");
//    string cmd_str = "folder_find(Libraries, C:/Users/12436/Desktop/1234/Examples, C:/Users/12436/Desktop/1234/Libraries, REPLACE);";
//    string cmd_str = "folder_remove(C:/Users/12436/Desktop/1234/Libraries/Include);";
//    string cmd_str = "folder_copy(C:/Users/12436/Desktop/1234/Libraries/Source, C:/Users/12436/Desktop/1234/Source);";

//    file_str_replace("C:/Users/12436/Desktop/1234/Libraries/Include/include.h", "tx8C1200", "tx8C126x");                  // �ļ������滻
//    folder_str_replace("C:/Users/12436/Desktop/1234/Examples", "TXM8C1200", "TX8C126x");                                  // �ļ����µ������ļ������滻
//    file_str_replace("C:/Users/12436/Desktop/1234/Libraries/Include/include.h", "tx8C126x_led.h", "");                    // ���˴���"tx8C126x_led.h"�ַ�������
//    folder_str_replace("C:/Users/12436/Desktop/1234/Examples", ".c" "tx8C126x_led.h", "");                                     // �����ļ����µ������ļ����ݴ���"tx8C126x_led.h"�ַ�������
//    file_section_remove("C:/Users/12436/Desktop/1234/Libraries/Include/tx8C1200_system.h", "__PMU_START__", "__PMU_END__"); // �ļ����ݶ���ɾ��

    string cmd_str;
    ifstream cmd_file_in("./FileMan.FileMan");

    while (getline(cmd_file_in, cmd_str)) {
        vector<string> p_cmd_str = cmd_parse(cmd_str);
        if (p_cmd_str.size() > 0) {
            cout << cmd_str << endl;

            if (p_cmd_str[0] == "folder_find") {
                if (p_cmd_str.size() > 4) {
                    folder_find(p_cmd_str[1], p_cmd_str[2], p_cmd_str[3], p_cmd_str[4]);
                }

            }
            else if (p_cmd_str[0] == "folder_remove") {
                if (p_cmd_str.size() > 1) {
                    folder_remove(p_cmd_str[1]);
                }

            }
            else if (p_cmd_str[0] == "folder_copy") {
                if (p_cmd_str.size() > 2) {
                    folder_copy(p_cmd_str[1], p_cmd_str[2]);
                }
            }
            else if (p_cmd_str[0] == "copy_file") {
                if (p_cmd_str.size() > 2) {
                    copy_file(p_cmd_str[1], p_cmd_str[2]);
                }
            }
            else if (p_cmd_str[0] == "remove_file") {
                if (p_cmd_str.size() > 1) {
                    remove_file(p_cmd_str[1]);
                }
            }
            else if (p_cmd_str[0] == "file_str_replace") {
                if (p_cmd_str.size() > 3) {
                    file_str_replace(p_cmd_str[1], p_cmd_str[2], p_cmd_str[3]);
                }
            }
            else if (p_cmd_str[0] == "folder_str_replace") {
                if (p_cmd_str.size() > 4) {
                    folder_str_replace(p_cmd_str[1], p_cmd_str[2], p_cmd_str[3], p_cmd_str[4]);
                }
            }
            else if (p_cmd_str[0] == "file_section_remove") {
                if (p_cmd_str.size() > 3) {
                    file_section_remove(p_cmd_str[1], p_cmd_str[2], p_cmd_str[3]);
                }
            }
        }
    }
    
    system("pause");

    return 0;
}

