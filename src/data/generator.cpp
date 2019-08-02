#include <cmath>
#include <cstring>

#include <iostream>
#include <fstream>
#include <limits>
#include <random>
#include <sstream>
#include <vector>
#include <unordered_map>

// For the command line parsing
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#define FEATUREVECTORS_FILE "data/datasets/chr22_feature.vectors"
#define GENES_FILE "data/datasets/genes.txt"
#define POWER_DATASET_FILE "data/datasets/DEBS2012-ChallengeData.txt"

#define DATA_FILE "data"
#define QUERY_FILE "queries"

void usage(){
  std::cout << std::endl;
  std::cout << "Usage:" <<std::endl;
  std::cout << "-r <number_of_rows>" << std::endl;
  std::cout << "-d <number_of_dimensions>" << std::endl;
  std::cout << "-w <workload_choice>" << std::endl;
  std::cout << "  '-(0=normal, 1=clustered, 2=uniform, 3=GMRQB, 4=power)" << std::endl;
  std::cout << "  '- GMRQB has fixed selectivity and 19 dimensions." << std::endl;
  std::cout << "  '- power has fixed 4 dimensions." << std::endl;
  std::cout << "-s <selectivity>" << std::endl;
  std::cout << "-q <number_of_queries>" << std::endl;
  std::cout << "-t <query_type>" << std::endl;
}

int main(int argc, char* argv[]) {
  int64_t n_of_rows = -1;
  int64_t dimensions = -1;
  int64_t workload = -1;
  float selectivity = -1;
  int64_t number_of_queries = -1;
  int64_t query_type = -1;

  int c;
  while ((c = getopt (argc, argv, "r:d:s:w:t:q:")) != -1){
      switch (c)
      {
      case 'w':
          workload = atoi(optarg);
          break;
      case 'r':
          n_of_rows = atoi(optarg);
          break;
      case 'd':
          dimensions = atoi(optarg);
          break;
      case 's':
          selectivity = atof(optarg);
          break;
      case 'q':
          number_of_queries = atoi(optarg);
          break;
      case 't':
          query_type = atoi(optarg);
          break;
      default:
          usage();
          exit(-1);
      }
  }

  if(n_of_rows == -1){
    std::cout << "Errors:" << std::endl;
    std::cout << "-r <n_of_rows> required" << std::endl;
    usage();
    exit(-1);
  }
  if(dimensions == -1){
    std::cout << "Errors:" << std::endl;
    std::cout << "-d <dimensions> required" << std::endl;
    usage();
    exit(-1);
  }
  if(workload == -1){
    std::cout << "Errors:" << std::endl;
    std::cout << "-w <workload> required" << std::endl;
    usage();
    exit(-1);
  }
  if(selectivity == -1){
    std::cout << "Errors:" << std::endl;
    std::cout << "-s <selectivity> required" << std::endl;
    usage();
    exit(-1);
  }
  if(number_of_queries == -1){
    std::cout << "Errors:" << std::endl;
    std::cout << "-q <number_of_queries> required" << std::endl;
    usage();
    exit(-1);
  }
  if(query_type == -1){
    query_type = 7;
    exit(-1);
  }

  if(workload == 3)
    std::cout << "INFO: " << n_of_rows << " vectors, " << 19 << " dimensions." << std::endl;
  else if(workload == 4)
    std::cout << "INFO: " << n_of_rows << " vectors, " << 4 << " dimensions." << std::endl;
  else
    std::cout << "INFO: " << n_of_rows << " vectors, " << dimensions << " dimensions." << std::endl;

  std::vector< std::vector<float> > data_points(n_of_rows, std::vector<float>(dimensions));

  // Get data or generate it
  if (workload == 3) {
    // GMRQB
    dimensions = 19;
    size_t i = 0;
    std::ifstream feature_vectors(FEATUREVECTORS_FILE);
    std::string line;
    std::string token;

    std::ofstream myfile(DATA_FILE);

    while (std::getline(feature_vectors, line) && i < n_of_rows) {
      std::vector<float> data_point(dimensions);
      std::vector<std::string> line_tokens;
      std::istringstream iss(line);
      while(std::getline(iss, token, ' '))
        line_tokens.push_back(token);
      // parse dimensions

      for (size_t j = 0; j < dimensions; ++j){
        data_point[j] = stof(line_tokens[j]);
        myfile << line_tokens[j];
        if(j + 1 != dimensions)
          myfile << " ";
      }
      data_points[i] = data_point;

      myfile << "\n";
      i++;
    }
    myfile.close();
  } else if(workload == 4) {
    // Power Dataset
    size_t i = 0;
    dimensions = 3;
    std::ifstream tuples(POWER_DATASET_FILE);
    std::string line;
    std::string token;

    std::ofstream myfile(DATA_FILE);

    while (std::getline(tuples, line) && i < n_of_rows) {
      std::vector<float> data_point(dimensions);
      std::vector<std::string> line_tokens;
      std::istringstream iss(line);
      while(std::getline(iss, token, '\t'))
        line_tokens.push_back(token);
      // parse attributes
      for (size_t j = 1; j < 4; ++j){
        data_point[j-1] = stof(line_tokens[j]);
        myfile << line_tokens[j];
        if(j + 1 != 4)
          myfile << " ";
      }
      myfile << "\n";
      data_points[i++] = data_point;
    }

    myfile.close();
  } else {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> nd(0.5,0.5);
    std::uniform_real_distribution<double> ud(0,1);
    // create multi-modal distribution
    size_t distribution_modals = 3;
    std::vector<std::normal_distribution<double> > mmd(distribution_modals);
    //  for (size_t i = 0; i < distribution_modals; ++i)
    mmd[0] = std::normal_distribution<double>(0.2, 0.2);
    mmd[1] = std::normal_distribution<double>(0.4, 0.2);
    mmd[2] = std::normal_distribution<double>(0.6, 0.2);

    std::ofstream myfile(DATA_FILE);

    for (size_t i = 0; i < n_of_rows; ++i) {
      std::vector<float> data_point(dimensions);
      for (size_t j = 0; j < dimensions; ++j) {
        if (workload == 0)
          data_point[j] = (float) nd(gen);
        else if (workload == 1)
          data_point[j] = (float) mmd[rand() % distribution_modals](gen);
        else
          data_point[j] = (float) ((rand() % 1000000) / 1000000.0);

        myfile << data_point[j];
        if(j + 1 != dimensions)
          myfile << " ";
      }
      myfile << "\n";
      data_points[i] = data_point;
    }
  }

  // random insertion order
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(data_points.begin(), data_points.end(), g);

  // Generate queries
  std::vector<std::vector<float> > lb_queries(number_of_queries, std::vector<float>(dimensions, std::numeric_limits<float>::min()));
  std::vector<std::vector<float> > ub_queries(number_of_queries, std::vector<float>(dimensions, std::numeric_limits<float>::max()));
  if (workload == 3) {
    size_t i = 0;
    std::ifstream genes(GENES_FILE);
    std::string line;
    std::string token;

    std::ofstream myfile(QUERY_FILE);

    std::vector<float> cols;
    while (std::getline(genes, line) && i < number_of_queries) {
      cols.resize(0);
      std::vector<std::string> line_tokens;
      std::istringstream iss(line);
      while(std::getline(iss, token, '\t'))
        line_tokens.push_back(token);
      // Query 2 (chromosome and position)
      lb_queries[i][5] = (float) 21;
      ub_queries[i][5] = (float) 22;
      lb_queries[i][6] = (float) std::stof(line_tokens[4]) - 100000.0;
      ub_queries[i][6] = (float) std::stof(line_tokens[5]) + 200000.0;
      int query_type_;
      if(query_type > 7)
        query_type_ = rand() % 8;
      else
      query_type_ = query_type;
      int rand_point = rand() % n_of_rows;

      // myfile << "SELECT * FROM variations WHERE";
      // myfile << " chromosome = '" << lb_queries[i][5] << "'";
      // myfile << " AND location BETWEEN  '" << lb_queries[i][6] << "' AND '" << ub_queries[i][6] << "'";

      switch (query_type_) {
        // Query 2
        case 1:
          // qual (create range around a certain qual found in the data set)
          lb_queries[i][8] = data_points[rand_point][8] * 0.5;
          ub_queries[i][8] = lb_queries[i][8] * 3;
          // myfile << " AND quality BETWEEN  '" << lb_queries[i][8] << "' AND '" << ub_queries[i][8] << "'";
          // depth (create range around a certain depth found in the data set)
          lb_queries[i][9] = data_points[rand_point][9] * 0.5;
          ub_queries[i][9] = lb_queries[i][9] * 3;
          // myfile << " AND depth BETWEEN  '" << lb_queries[i][9] << "' AND '" << ub_queries[i][9] << "'";
          // allele freq (create range using a certain allele_freq found in the data set)
          lb_queries[i][10] = data_points[rand_point][10];
          ub_queries[i][10] = lb_queries[i][10] + 0.3;
          // myfile << " AND allele_freq BETWEEN  '" << lb_queries[i][10] << "' AND '" << ub_queries[i][10] << "'";
          cols.push_back(5);
          cols.push_back(6);
          cols.push_back(8);
          cols.push_back(9);
          cols.push_back(10);
          break;
        // Query 3
        case 2:
          // gender
          lb_queries[i][2] = data_points[rand_point][2];
          ub_queries[i][2] = lb_queries[i][2];
          // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
          cols.push_back(5);
          cols.push_back(6);
          cols.push_back(2);
          break;
        // Query 4
        case 3:
          // gender
          lb_queries[i][2] = data_points[rand_point][2];
          ub_queries[i][2] = lb_queries[i][2];
          // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
          // population
          lb_queries[i][1] = data_points[rand_point][1];
          ub_queries[i][1] = lb_queries[i][1];
          // myfile << " AND population =  '" << lb_queries[i][1] << "'";
          cols.push_back(5);
          cols.push_back(6);
          cols.push_back(2);
          cols.push_back(1);
          break;
        // Query 5
        case 4:
          // gender
          lb_queries[i][2] = data_points[rand_point][2];
          ub_queries[i][2] = lb_queries[i][2];
          // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
          // population
          lb_queries[i][1] = data_points[rand_point][1];
          ub_queries[i][1] = lb_queries[i][1];
          // myfile << " AND population =  '" << lb_queries[i][1] << "'";
          // relationship
          lb_queries[i][4] = data_points[rand_point][4];
          ub_queries[i][4] = lb_queries[i][4];
          // myfile << " AND relationship =  '" << lb_queries[i][4] << "'";
          cols.push_back(5);
          cols.push_back(6);
          cols.push_back(2);
          cols.push_back(1);
          cols.push_back(4);
          break;
        // Query 6
        case 5:
          // gender
          lb_queries[i][2] = data_points[rand_point][2];
          ub_queries[i][2] = lb_queries[i][2];
          // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
          // population
          lb_queries[i][1] = data_points[rand_point][1];
          ub_queries[i][1] = lb_queries[i][1];
          // myfile << " AND population =  '" << lb_queries[i][1] << "'";
          // relationship
          lb_queries[i][4] = data_points[rand_point][4];
          ub_queries[i][4] = lb_queries[i][4];
          // myfile << " AND relationship =  '" << lb_queries[i][4] << "'";
          // family_id (create range using a certain family_id found in the data set)
          lb_queries[i][3] = data_points[rand_point][3] * 0.5;
          ub_queries[i][3] = lb_queries[i][3] * 3;
          // myfile << " AND family_id BETWEEN  '" << lb_queries[i][3] << "' AND '" << ub_queries[i][3] << "'";
          cols.push_back(5);
          cols.push_back(6);
          cols.push_back(2);
          cols.push_back(1);
          cols.push_back(4);
          cols.push_back(3);
          break;
        // Query 7
        case 6:
          // gender
          lb_queries[i][2] = data_points[rand_point][2];
          ub_queries[i][2] = lb_queries[i][2];
          // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
          // population
          lb_queries[i][1] = data_points[rand_point][1];
          ub_queries[i][1] = lb_queries[i][1];
          // myfile << " AND population =  '" << lb_queries[i][1] << "'";
          // relationship
          lb_queries[i][4] = data_points[rand_point][4];
          ub_queries[i][4] = lb_queries[i][4];
          // myfile << " AND relationship =  '" << lb_queries[i][4] << "'";
          // family_id (create range using a certain family_id found in the data set)
          lb_queries[i][3] = data_points[rand_point][3] * 0.5;
          ub_queries[i][3] = lb_queries[i][3] * 3;
          // myfile << " AND family_id BETWEEN  '" << lb_queries[i][3] << "' AND '" << ub_queries[i][3] << "'";
          // mutation_id (create range using a certain mutation_id found in the data set)
          lb_queries[i][7] = data_points[rand_point][7] * 0.5;
          ub_queries[i][7] = lb_queries[i][7] * 3;
          // myfile << " AND variation_id BETWEEN  '" << lb_queries[i][7] << "' AND '" << ub_queries[i][7] << "'";
          cols.push_back(5);
          cols.push_back(6);
          cols.push_back(2);
          cols.push_back(1);
          cols.push_back(4);
          cols.push_back(3);
          cols.push_back(7);
          break;
        // Query 8
        case 7:
          // qual (create range around a certain qual found in the data set)
          lb_queries[i][8] = data_points[rand_point][8] * 0.5;
          ub_queries[i][8] = lb_queries[i][8] * 3;
          // myfile << " AND quality BETWEEN  '" << lb_queries[i][8] << "' AND '" << ub_queries[i][8] << "'";
          // depth (create range around a certain depth found in the data set)
          lb_queries[i][9] = data_points[rand_point][9] * 0.5;
          ub_queries[i][9] = lb_queries[i][9] * 3;
          // myfile << " AND depth BETWEEN  '" << lb_queries[i][9] << "' AND '" << ub_queries[i][9] << "'";
          // allele freq (create range using a certain allele_freq found in the data set)
          lb_queries[i][10] = data_points[rand_point][10];
          ub_queries[i][10] = lb_queries[i][10] + 0.3;
          // myfile << " AND allele_freq BETWEEN  '" << lb_queries[i][10] << "' AND '" << ub_queries[i][10] << "'";
          // ref_base
          lb_queries[i][13] = data_points[rand_point][13];
          ub_queries[i][13] = lb_queries[i][13];
          // myfile << " AND ref_base =  '" << lb_queries[i][13] << "'";
          // alt_base
          lb_queries[i][14] = data_points[rand_point][14];
          ub_queries[i][14] = lb_queries[i][14];
          // myfile << " AND alt_base =  '" << lb_queries[i][14] << "'";
          // ancestral_allele
          lb_queries[i][16] = data_points[rand_point][16];
          ub_queries[i][16] = lb_queries[i][16];
          // myfile << " AND ancestral_allele =  '" << lb_queries[i][16] << "'";
          // mutation_id (create range using a certain mutation_id found in the data set)
          lb_queries[i][7] = data_points[rand_point][7] * 0.5;
          ub_queries[i][7] = lb_queries[i][7] * 3;
          // myfile << " AND variation_id BETWEEN  '" << lb_queries[i][7] << "' AND '" << ub_queries[i][7] << "'";
          // sample_id
          lb_queries[i][0] = data_points[rand_point][0];
          ub_queries[i][0] = lb_queries[i][0];
          // myfile << " AND sample_id BETWEEN  '" << lb_queries[i][0] << "' AND '" << ub_queries[i][0] << "'";
          // gender
          lb_queries[i][2] = data_points[rand_point][2];
          ub_queries[i][2] = lb_queries[i][2];
          // myfile << " AND gender =  '" << lb_queries[i][2] << "'";
          // family_id (create range using a certain family_id found in the data set)
          lb_queries[i][3] = data_points[rand_point][3] * 0.5;
          ub_queries[i][3] = lb_queries[i][3] * 3;
          // myfile << " AND family_id BETWEEN  '" << lb_queries[i][3] << "' AND '" << ub_queries[i][3] << "'";
          // population
          lb_queries[i][1] = data_points[rand_point][1];
          ub_queries[i][1] = lb_queries[i][1];
          // myfile << " AND population =  '" << lb_queries[i][1] << "'";
          // relationship
          lb_queries[i][4] = data_points[rand_point][4];
          ub_queries[i][4] = lb_queries[i][4];
          // myfile << " AND relationship =  '" << lb_queries[i][4] << "'";
          // variant_type
          lb_queries[i][15] = data_points[rand_point][15];
          ub_queries[i][15] = lb_queries[i][15];
          // myfile << " AND variant_type =  '" << lb_queries[i][15] << "'";
          // genotypegenotype
          lb_queries[i][17] = data_points[rand_point][17];
          ub_queries[i][17] = lb_queries[i][17];
          // myfile << " AND genotype =  '" << lb_queries[i][17] << "'";
          // allele_count (create range using a certain allele_count found in the data set)
          lb_queries[i][11] = data_points[rand_point][11] * 0.5;
          ub_queries[i][11] = lb_queries[i][11] * 3;
          // myfile << " BUG";
          // filter
          lb_queries[i][12] = data_points[rand_point][12];
          ub_queries[i][12] = lb_queries[i][12];
          // myfile << " BUG";
          // reference genome
          lb_queries[i][18] = data_points[rand_point][18];
          ub_queries[i][18] = lb_queries[i][18];
          // myfile << " BUG";
          cols.push_back(5);
          cols.push_back(6);
          cols.push_back(8);
          cols.push_back(9);
          cols.push_back(10);
          cols.push_back(13);
          cols.push_back(14);
          cols.push_back(16);
          cols.push_back(7);
          cols.push_back(0);
          cols.push_back(2);
          cols.push_back(3);
          cols.push_back(1);
          cols.push_back(4);
          cols.push_back(15);
          cols.push_back(17);
          cols.push_back(11);
          cols.push_back(12);
          cols.push_back(18);
          break;
        // Query 1
        case 0:
          cols.push_back(5);
          cols.push_back(6);
        default:
          break;
      }
      for(size_t col = 0; col < cols.size(); ++col){
        myfile << lb_queries[i][cols[col]];
        if(col + 1 != cols.size())
          myfile << " ";
      }
      myfile << "\n";

      for(size_t col = 0; col < cols.size(); ++col){
        myfile << ub_queries[i][cols[col]];
        if(col + 1 != cols.size())
          myfile << " ";
      }
      myfile << "\n";

      for(size_t col = 0; col < cols.size(); ++col){
        myfile << cols[col];
        if(col + 1 != cols.size())
          myfile << " ";
      }
      myfile << "\n";
      i++;
    }
    myfile.close();
  } else {
    std::ofstream myfile(QUERY_FILE);
    for (size_t i = 0; i < number_of_queries; ++i) {
      // myfile << "SELECT * FROM synthetic WHERE";
      int first  = rand() % n_of_rows;
      int second = rand() % n_of_rows;
      for (size_t j = 0; j < dimensions; ++j) {
        lb_queries[i][j] = std::min(data_points[first][j], data_points[second][j]);
        ub_queries[i][j] = std::max(data_points[first][j], data_points[second][j]);
        if (workload == 2) {
          lb_queries[i][j] = (float) ((rand() % (10000)) / 1000000.0);
          ub_queries[i][j] = lb_queries[i][j] + selectivity;
        }
      }
      for(size_t col = 0; col < dimensions; ++col){
        myfile << lb_queries[i][col];
        if(col + 1 != dimensions)
          myfile << " ";
      }
      myfile << "\n";

      for(size_t col = 0; col < dimensions; ++col){
        myfile << ub_queries[i][col];
        if(col + 1 != dimensions)
          myfile << " ";
      }
      myfile << "\n";

      for(size_t col = 0; col < dimensions; ++col){
        myfile << col;
        if(col + 1 != dimensions)
          myfile << " ";
      }
      myfile << "\n";
    }
    myfile.close();
  }

  return 0;
}
