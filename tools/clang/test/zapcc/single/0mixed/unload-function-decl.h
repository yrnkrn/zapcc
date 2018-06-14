#pragma once
template <typename T> class Bug2550future { T _local_state; };
class Bug2550file;
Bug2550future<Bug2550file> Bug2550open_file_dma();
