//
// Created by Kacper Smykla on 26.08.2026.
//
#include "G1_to_PP.h"
#include "G2_to_G3.h"
#include "Visualization.h"

constexpr std::string PROJECT_DIR = "..";

void testMainLoop() {
    TS_G1_to_PP g1_to_pp = from_file_ts_G1_to_PP(PROJECT_DIR + "/test/data/G1toPP.suite.json");
    TSR_G1_to_PP res_g1_to_pp = run_ts_G1_to_PP(g1_to_pp, true);
    std::ofstream ofs(PROJECT_DIR + "/test/data/G1toPP.suite.out.json");
    ofs << res_g1_to_pp.to_json_string();
    ofs.close();

    TS_PP_to_G2 pp_to_g2 = from_file_ts_PP_to_G2(PROJECT_DIR + "/test/data/PPtoG2.suite.json");
    TSR_PP_to_G2 res_pp_to_g2 = run_ts_PP_to_G2(pp_to_g2, true);
    std::ofstream ofs2(PROJECT_DIR + "/test/data/PPtoG2.suite.out.json");
    ofs2 << res_pp_to_g2.to_json_string();
    ofs2.close();

    TS_G2_to_G3 g2_to_g3 = from_file_ts_G2_to_G3(PROJECT_DIR + "/test/data/G2toG3.suite.json");
    TSR_G2_to_G3 res_g2_to_g3 = run_ts_G2_to_G3(g2_to_g3, true);
    std::ofstream ofs3(PROJECT_DIR + "/test/data/G2toG3.suite.out.json");
    ofs3 << res_g2_to_g3.to_json_string();
    ofs3.close();

    // test 1 imgs
    for (int id = 0; id < res_g1_to_pp.results.size(); id++) {
        std::cout << "Generating input image for G1 to PP case no. " << id << std::endl;
        png_from_dot(adj_to_dot(res_g1_to_pp.results[id].test_case.in_graph), PROJECT_DIR + "/test/data/G1toPPpngs/"  + std::to_string(id) + ".in.png");
    }

    for (int id = 0; id < res_pp_to_g2.results.size(); id++) {
        std::cout << "Generating input image for PP to G2 case no. " << id << std::endl;
        png_from_dot(adj_to_dot(res_pp_to_g2.results[id].test_case.in_graph), PROJECT_DIR + "/test/data/PPtoG2pngs/"  + std::to_string(id) + ".in.png");
        std::cout << "Generating output image for PP to G2 case no. " << id << std::endl;
        png_from_dot(adj_to_dot(res_pp_to_g2.results[id].output_graph), PROJECT_DIR + "/test/data/PPtoG2pngs/"  + std::to_string(id) + ".out.png");
    }

    for (int id = 0; id < res_g2_to_g3.results.size(); id++) {
        std::cout << "Generating input image for G2 to G3 case no. " << id << std::endl;
        png_from_dot(adj_to_dot(res_g2_to_g3.results[id].test_case.in_graph), PROJECT_DIR + "/test/data/G2toG3pngs/"  + std::to_string(id) + ".in.png");
        std::cout << "Generating output image for G2 to G3 case no. " << id << std::endl;
        png_from_dot(adj_to_dot(res_g2_to_g3.results[id].output_graph), PROJECT_DIR + "/test/data/G2toG3pngs/"  + std::to_string(id) + ".out.png");
    }


    std::vector<int> failedIDs;
    for (int i = 0; i < res_g2_to_g3.results.size(); i++) {
        if (!res_g2_to_g3.results[i].success) failedIDs.push_back(i);
    }

    std::cout << "Printing IDs of graph in which removing cycles failed: ";
    for (auto id : failedIDs) std::cout << id << " " << std::endl;
    std::cout << std::endl << std::endl;
}

int main() {
    testMainLoop();
    return 0;
}
