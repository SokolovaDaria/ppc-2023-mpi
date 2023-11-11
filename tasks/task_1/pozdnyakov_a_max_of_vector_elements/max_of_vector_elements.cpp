// Copyright 2023 Pozdnyakov Vasya
#include <random>
#include "task_1/pozdnyakov_a_max_of_vector_elements/max_of_vector_elements.h"

int get_max_element(const std::vector<int>& init_vec, size_t vec_size) {
    boost::mpi::communicator comm;
    int result = 0;
    const size_t part_size = vec_size / comm.size();
    const size_t remain = vec_size % comm.size();
    std::vector<int> local_vec_sizes(comm.size(), static_cast<int>(part_size));
    std::vector<int> local_vec(part_size);
    local_vec_sizes[0] += static_cast<int>(remain);

    if (part_size == 0) {
        local_vec = init_vec;
    } else {
        if (comm.rank() == 0) {
            local_vec.resize(local_vec_sizes[0]);
            boost::mpi::scatterv(comm, init_vec, local_vec_sizes, local_vec.data(), 0);
        } else {
            boost::mpi::scatterv(comm, local_vec.data(), local_vec_sizes[comm.rank()], 0);
        }
    }

    auto max_it = std::max_element(local_vec.begin(), local_vec.end());

    if (part_size == 0) {
        if (comm.rank() == 0) {
            result = *max_it;
        }
    } else {
        boost::mpi::reduce(comm, *max_it, result, boost::mpi::maximum<int>(), 0);
    }

    return result;
}

std::vector<int> get_random_vector(size_t size, int min_elem, int max_elem) {
    std::random_device dev;
    std::mt19937 gen(dev());
    std::uniform_int_distribution<int> distrib(min_elem, max_elem);

    std::vector<int> vec(size);
    for (int  i = 0; i < size; i++) {
        vec[i] = distrib(gen);
    }
    return vec;
}
