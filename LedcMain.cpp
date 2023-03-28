/* ************************************************************************
 *
 * Copyright (C) 2022 Vincent Luo All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ************************************************************************/

 /* Creates on 2023/3/28. */
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

unsigned int SLICE_BUFFER_MALLOC = 1 * (1024 * 1024 * 1024);

/**
 * generate slice file name.
 */
std::string number_of_slice_name(const std::string &name, int num)
{
    std::stringstream ss;
    ss << name;
    ss << "_";
    ss << num;
    ss << ".ldc";

    return ss.str();
}

/**
 * file slice.
 */
void slice(std::ifstream *fs, const std::string &name, size_t size)
{
    printf("    file size larger than 2g, start file slices.\n");
    // reset seekg of fs.
    fs->seekg(0, std::ios::beg);

    // number of file slices.
    int slices_num = (int) (size / SLICE_BUFFER_MALLOC);
    printf("    number of file slices: %d\n", slices_num);

    // allocate a 2g size buffer.
    printf("    begin file slices, file size count: %zu\n", size);
    size_t size_of_rem = size;
    for (int i = 0; i < slices_num; i++) {
        unsigned int size_of_read =
                size_of_rem > SLICE_BUFFER_MALLOC ? SLICE_BUFFER_MALLOC : size_of_rem;

        // read
        char *buffer = (char *) malloc(size_of_read);
        fs->read(buffer, SLICE_BUFFER_MALLOC);

        // write
        std::ofstream wf;
        wf.open(number_of_slice_name(name, i), std::ios::out | std::ios::binary);
        wf.write(buffer, SLICE_BUFFER_MALLOC);
        wf.close();

        // finally
        size_of_rem -= size_of_read;
        printf("    [%d] - slice success. write size: %u, rem size: %zu\n", i, size_of_read, size_of_rem);
    }
}

/**
 * file mergers.
 */
void mergers(const std::string &first)
{
    // TODO
}

/**
 * ledc main function and ignore main parameters.
 */
int main(int argc, char **argv)
{
	if (argc < 1)
		return 0;

    char *name = argv[1];

	printf("ledc application runing: \n");
	std::ifstream fs;
    fs.open(name, std::ios::in | std::ios::binary);

	// is open success!
	if (!fs.is_open()) {
		fprintf(stderr, "Error: failed open [%s] file\n", name);
		return -1;
	}

    printf("    open [%s] file success\n", name);

    fs.seekg(0, std::ios::end);
    size_t size = fs.tellg();
    fs.seekg(0, std::ios::beg);

    double uintsiz = ((double) size / (1024 * 1024 * 1024));
    printf("    file size: %.2fGB\n", uintsiz);

    // slice the file if size is larger than 2g.
    if (uintsiz > 2.0f)
        slice(&fs, name, size);

    // close fs.
    fs.close();

	return 0;
}