/****************************************************************************
  FileName     [ manager.cpp ]
  Synopsis     [ Implementation of a Intersection Manager ]
  Author       [ Ting-Wei (Andy) Liu ]
  Copyright    [ Copyleft(c), NTUEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include <vector>
#include <algorithm>
using namespace std;


/**********************************/
/*          Declarations          */
/**********************************/
#define DEBUG
#define n_dir 4
static string str_buf;
static const string directions[n_dir] = { "N:", "E:", "S:", "W:" };


/*******************************/
/*        class Manager        */
/*******************************/
/*
Operation rules:
1. car gets to go through the crossroad according to its rank
2. car with direction covering more of the crossroad has a higher rank
3. drop a car's rank to lowest if: 1) it is the last car in that direction, 2) there is no car behind it
4. move the waiting line forward for one space if a previous car is delayed
*/
class Manager
{
public:
    Manager() {
        _input_table.reserve(n_dir);
        _result.reserve(n_dir);
        _queue_empty = new bool[n_dir];
        _queue_confict = new bool[n_dir];
        _rank_list = new int[n_dir];
        _delay = 0;
        for (int i = 0; i < n_dir; ++i) {
        	_queue[i] = INT_MAX;
        	_queue_empty[i] = true;
        	_result.push_back(vector<int>());
        }
    }

    ~Manager() { delete[] _queue_empty;
    			 delete[] _queue_confict;
    			 delete[] _rank_list; }

    // main function
    void process();

    // utility functions
    void read_input (const string& input_file);
    void save_output(const string& output_file) const;
    void display_input (bool show_input) const;
    void display_output (bool show_output) const;
    bool valid_checker() const;

private:
    // operation tables
    static const int            _rank_table[n_dir][n_dir];
    static const int            _conf_table[n_dir][n_dir][n_dir];
    
    // data storage
    vector<vector<int> >        _input_table; // shape: [4, N]
    vector<vector<int> >        _result;      // shape: [4, N]

    // operation variabls
    int                         _queue[n_dir];
    bool*                       _queue_empty;
    bool*                       _queue_confict;
    int*                        _rank_list;
    #ifdef DEBUG
    int                         _delay;
    int                         _n_car;
    #endif

    /******************************************/

    // core functions
    bool car_in_line() const;
    void get_next();
    void update_wait_line(vector<int>& vec);
    bool resolve(int (&output_list)[n_dir]);
    void check_conflict() const;
    int decide() const;

    // helper functions:
    int dir2int (const string& direction) const;
    string int2dir (const int& direction) const;
    void add_arr(int (&arr1)[n_dir], const int (&arr2)[n_dir]) const;
    int run_check(int (&arr)[n_dir]) const;
};


/****************************************************/
/*        class Manager array initialization        */
/****************************************************/
const int Manager::_rank_table[n_dir][n_dir] =
    // rank values: 3 > 2 > 1 > 0
    // higher ranking gets to go first
    // index mapping: <0-N>, <1-E>, <2-S>, <3-W>
{	
    {0, 3, 2, 1}, // North: {'N':rank 0, 'E':rank 3, 'S':rank 2, 'W':rank 1}
    {1, 0, 3, 2}, // East:  {'N':rank 1, 'E':rank 0, 'S':rank 3, 'W':rank 2}
    {2, 1, 0, 3}, // South: {'N':rank 2, 'E':rank 1, 'S':rank 0, 'W':rank 3}
    {3, 2, 1, 0}, // West:  {'N':rank 3, 'E':rank 2, 'S':rank 1, 'W':rank 0}
};


const int Manager::_conf_table[n_dir][n_dir][n_dir] =
    // conflict table mapping:
    //     |   |        
    //  -  1   2  -
    //  -  3   4  -  map to: [1, 2, 3, 4]
    //     |   |
    // index mapping: <0-N>, <1-E>, <2-S>, <3-W>
{ 
  { {0, 0, 0, 0}, {1, 0, 1, 1}, {1, 0, 1, 0}, {1, 0, 0, 0} }, // North: { {'empty':00}, {'E':1E}, {'S':1S}, {'W':1W} }
  { {0, 1, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 0}, {1, 1, 0, 0} }, // East:  { {'N':1N}, {'empty':00}, {'S':1S}, {'W':1W} }
  { {0, 1, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 0}, {1, 1, 0, 1} }, // South: { {'N':1N}, {'E':1E}, {'empty':00}, {'W':1W} }
  { {0, 1, 1, 1}, {0, 0, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0} }  // West:  { {'N':1N}, {'E':1E}, {'S':1S}, {'empty':00} }
};


/**********************************************/
/*        class Manager main function        */
/**********************************************/
void Manager::process()
{   
    cout << "Processing traffic..." << endl;
    int count = 0;
    while(car_in_line()){

    	cout << "Iteration: " << ++count << '\r';
    	// reset and initialize
    	for (int i = 0; i < n_dir; ++i) _rank_list[i] = 0;
    	int output_list[n_dir] = {4, 4, 4, 4};

        get_next();
        bool resolved = resolve(output_list);
        
        if (!resolved) {
        	cout << "00000" << endl;
            check_conflict();

            // compute rank list
            for (int i = 0; i < n_dir; ++i) {
            	if (_queue_confict[i] == true) { // for the cars that have conflict
            		if (_input_table.at(i).size() == 0) _rank_list[i] = 1; // lower its ranking to lowest if it is the last car in that direction
            		else if (_input_table.at(i).back() == 4) _rank_list[i] = 1; //lower its ranking to lowest if one car space behind it
            		else _rank_list[i] = _rank_table[i][_queue[i]]; // get the original rank of the car
            	}
            	else if (_queue_empty[i] == false) {
            		output_list[i] = _queue[i]; // output that car if it has no conflict with others
            		_queue_empty[i] = true; // clear queue
            	}
            }

            // resolve conflict base on rank list
            int decision = decide();
            if (decision != INT_MAX) { // already handled the case where all cars has no conflict
            	output_list[decision] = _queue[decision];
            	_queue_empty[decision] = true;
            }
        }

        for (int i = 0; i < n_dir; ++i)
        	_result.at(i).push_back(output_list[i]);
    }
    cout << endl;
}


/**********************************************/
/*        class Manager core functions        */
/**********************************************/
bool Manager::car_in_line() const
{
    // check whether if there are cars still waiting to be processed, check in queue and input table
    for (int i = 0; i < n_dir; ++i)
        if ((_input_table.at(i).size() > 0) || (_queue_empty[i] == false))
            return true;
    return false;
}


void Manager::get_next()
{
    for (int i = 0; i < n_dir; ++i) {
        if ((_queue_empty[i] == true) && (_input_table.at(i).size() > 0)) {   
            _queue_empty[i] = false;
            _queue[i] = _input_table.at(i).back();
            _input_table.at(i).pop_back();
            #ifdef DEBUG
            if (_queue[i] != 4) _n_car += 1;
            #endif
        }
        else if (_queue_empty[i] == false) {
        	#ifdef DEBUG
            _delay += 1;
            #endif
            update_wait_line(_input_table.at(i)); // if the previous car is delayed, remove the first encounter trailing "00"
        }
    }
}


void Manager::update_wait_line(vector<int>& vec)
{   
    // call this function when a previous car was delayed, move the line forward, delete the first encountered "00"
    if (vec.size() == 0) return;
    for (vector<int>::iterator it = vec.end()-1; it != vec.begin(); --it) {
        if (*it == 4) {
            vec.erase(it); 
            return;
        }
    }
    if (*vec.begin() == 4)
        vec.erase(vec.begin());
}


bool Manager::resolve(int (&output_list)[n_dir])
{   
    // check pairs of 4
	if ((_queue_empty[0] == false) && (_queue_empty[1] == false) && (_queue_empty[2] == false) && (_queue_empty[3] == false)) {
	    int check4[4] = {0, 0, 0, 0};
	    for (int i = 0; i < n_dir; ++i)
	        if (_queue[i] != 4) add_arr(check4, _conf_table[i][_queue[i]]);
	    if (run_check(check4) != 0) {
	        for (int j = 0; j < n_dir; ++j) {
	            output_list[j] = _queue[j];
	            _queue_empty[j] = true;
	        }
	        cout << "4" << endl;
	        return true;
	    }
	}

	int check_sum = 0;
	int check_sum2 = 0;
	int idx = INT_MAX;
	int idx2 = INT_MAX;

    // check pairs of 3
    for (int i = 0; i < n_dir; ++i) {
    	int non_empty_3 = 0;
    	for (int j = 0; j < n_dir; ++j)
    		if ((i != j) && (_queue_empty[j] == false)) non_empty_3 += 1;
    	if (non_empty_3 == 3) { // check 3 non empty queue
	        int check[4] = {0, 0, 0, 0};
	        for (int j = 0; j < n_dir; ++j)
	            if ((i != j) && (_queue[j] != 4)) add_arr(check, _conf_table[j][_queue[j]]);
	        int sum = run_check(check);
	        if (sum > check_sum) { // finding the best pair of 3
	        	check_sum = sum;
	        	idx = i;
	        }
    	}
    }
    if (check_sum > 0) { // finding the best pair of 3
    	for (int i = 0; i < n_dir; ++i)
	        if (idx != i) {
	            output_list[i] = _queue[i];
	            _queue_empty[i] = true;
	        }
	    if (_queue[idx] == 4) _queue_empty[idx] = true;
	    cout << "3" << endl;
	    return true;
    }

    // check pairs of 2, case 1
    for (int i = 0; i < n_dir-1; ++i) {
    	if ((_queue_empty[i] != true) && (_queue_empty[i+1] != true)) { // skip empty queue
	        int check[4] = {0, 0, 0, 0};
	        if (_queue[i] != 4) add_arr(check, _conf_table[i][_queue[i]]);
	        if (_queue[i+1] != 4) add_arr(check, _conf_table[i+1][_queue[i+1]]);
	        int sum = run_check(check);
	        if (sum > check_sum) { // finding the best pair of 3
	        	check_sum = sum;
	        	idx = i;
	        }
    	}
    }
    // check pairs of 2, case 2
    for (int i = 0; i < n_dir-2; ++i) {
    	if ((_queue_empty[i] != true) && (_queue_empty[i+2] != true)) { // skip empty queue
	        int check[4] = {0, 0, 0, 0};
	        if (_queue[i] != 4) add_arr(check, _conf_table[i][_queue[i]]);
	        if (_queue[i+2] != 4) add_arr(check, _conf_table[i+2][_queue[i+2]]);
	        int sum = run_check(check);
	        if (sum > check_sum2) { // finding the best pair of 3
	        	check_sum2 = sum;
	        	idx2 = i;
	        }
    	}
    }

    // find best pair of 2
    if (check_sum > check_sum2) {
	    if (check_sum > 0) {
		    output_list[idx] = _queue[idx];
		    output_list[idx+1] = _queue[idx+1];
		    _queue_empty[idx] = true;
		    _queue_empty[idx+1] = true;
		    for (int i = 0; i < n_dir; ++i) if (_queue[i] == 4) _queue_empty[i] = true;
		    cout << "2" << endl;
		    return true;
	    }
	}
	else {
		check_sum = check_sum2;
		idx = idx2;
	    if (check_sum > 0) {
		    output_list[idx] = _queue[idx];
		    output_list[idx+2] = _queue[idx+2];
		    _queue_empty[idx] = true;
		    _queue_empty[idx+2] = true;
		    for (int i = 0; i < n_dir; ++i) if (_queue[i] == 4) _queue_empty[i] = true;
		    cout << "1" << endl;
		    return true;
	    }
	}
    return false;
}


void Manager::check_conflict() const
{   
    for (int i = 0; i < n_dir; ++i) {
    	_queue_confict[i] = false; // reset
        if ((_queue_empty[i] == false) && (_queue[i] != 4)) { //check conflict for non-empty queue, dont have to check for "00"
            for (int j = 0; j < n_dir; ++j) {
	           int check[4] = {0, 0, 0, 0}; // reset check every time
	           add_arr(check, _conf_table[i][_queue[i]]);
	            if ((i != j) && (_queue_empty[j] == false)) { // avoid checking itself and empty queue
	                add_arr(check, _conf_table[j][_queue[j]]);
	                for (int k = 0; k < n_dir; ++k)
	                    if (check[k] > 1)
	                        _queue_confict[i] = true;
	                if (_queue_confict[i] == true) break;
	            }
	        }
    	}
    }    
}


int Manager::decide() const
{	
	// case where all cars have no conflict, doesn't have to decide
	int sum = 0;
	for (int i = 1; i < n_dir; ++i)
		sum += _rank_list[i];
	if (sum == 0) return INT_MAX;

	// makes a decision
	int decision = 0;
	int highest_rank = _rank_list[0];
	for (int i = 1; i < n_dir; ++i)
		if (_rank_list[i] > highest_rank) {// find highest rank
			decision = i;
			highest_rank = _rank_list[i];
		}
		else if (_rank_list[i] == highest_rank) {
			if (_input_table.at(i).size() > _input_table.at(decision).size()) { // if same rank, update decision if it has a longer line
				decision = i;
				highest_rank = _rank_list[i];
			}
		}
	#ifdef DEBUG
	assert((0 <= decision) && (decision <= 3));
	#endif
	return decision;
}


/*************************************************/
/*        class Manager utility functions        */
/*************************************************/
void Manager::read_input (const string& input_file)
{
    // opening file
    ifstream ifs(input_file.c_str());
    #ifdef DEBUG
    if (!ifs) cerr << "Cannot open file \"" << input_file << "\"!" << endl;
    assert(ifs);
    #endif

    // read 4 lines for 4 directions
    for (int i = 0; i < n_dir; ++i) {
        getline(ifs, str_buf, '\n');
        
        int begin = 0;
        int end = 0;
        string holder;
        vector<int> line;
        
        // parse each line into tokens
        for (string::iterator it = str_buf.begin(); it != str_buf.end(); ++it) {
            if ((*it == ' ') || (*it == '\t')) {
            	holder = str_buf.substr(begin, end - begin);
            	#ifdef DEBUG
            	assert(holder.size() == 2);
            	#endif
            	if (holder != directions[i]) { // discard the direction label
            		line.push_back(dir2int(holder));
            	}
                begin = end + 1;
            }
            end += 1;
        }

        // parse the last token
        holder = str_buf.substr(begin, end - begin); // the last line
        if (holder.size() > 2) holder = holder.substr(0, 2);
        #ifdef DEBUG
        assert(holder.size() == 2);
        #endif
        line.push_back(dir2int(holder));
        _input_table.push_back(line);
    }

    cout << "Number of rounds: " << _input_table.at(0).size() << endl;

    for (int i = 0; i < n_dir; ++i) {
    	while(_input_table.at(i).back() == n_dir) {
    		_input_table.at(i).pop_back(); // discard all trailing 00's
    		if (_input_table.at(i).size() < 1) break;
    	}
        reverse(_input_table.at(i).begin(), _input_table.at(i).end()); // reverse the order for pop_back in get_next()   
    }
}


void Manager::display_input(bool show_input) const
{
    if (show_input == true) {
        vector< vector<int> > input_table = _input_table;
        cout << "Input: " << endl;
        for (int i = 0; i < input_table.size(); ++i) {
            reverse(input_table.at(i).begin(), input_table.at(i).end()); // reverse to original order for printing
        	cout << directions[i];
            for (int j = 0; j < input_table.at(i).size(); ++j)
                cout << ' ' << int2dir(input_table.at(i).at(j));
            cout << '.' << endl;
        }
    }
}


void Manager::display_output(bool show_output) const
{   
    if (show_output == true) {
        cout << "Result: " << endl;
        for (int i = 0; i < _result.size(); ++i) {
            cout << directions[i] ;
            for (int j = 0; j < _result.at(i).size(); ++j)
                cout << ' ' << int2dir(_result.at(i).at(j));
            cout << '\n';
        }
    }
    #ifdef DEBUG
    cout << "==================== Summary ====================" << endl;
    cout << "Total delay: " << _delay << endl;
    cout << "Total car number: " << _n_car << endl;
    cout << "Total rounds spent: " << _result.at(0).size() << endl;
    cout << "=================================================" << endl;
    #endif
}


void Manager::save_output(const string& output_file) const {
    ofstream file;
    file.open (output_file.c_str());

    for (int i = 0; i < _result.size(); ++i) {
        file << directions[i] ;
        for (int j = 0; j < _result.at(i).size(); ++j)
            file << ' ' << int2dir(_result.at(i).at(j));
        file << '\n';
    }

    file.close();
    cout << "Result successfully saved to: " << output_file << endl;
}


bool Manager::valid_checker() const
{   
    int count = 0;
    for (int i = 0; i < _result.at(0).size(); ++i) {
        int check[4] = {0, 0, 0, 0};
        for (int j = 0; j < n_dir; ++j)
            if (_result.at(j).at(i) != 4) {
                add_arr(check, _conf_table[j][_result.at(j).at(i)]);
                count += 1;
            }
        for (int j = 0; j < n_dir; ++j)
            if (check[j] > 1) return false;
    }
    if (count != _n_car) return false;
    return true;
}


/************************************************/
/*        class Manager helper functions        */
/************************************************/
int Manager::dir2int(const string& direction) const
{   
    if (direction == "00") return 4;
    if (direction == "1N") return 0;
    if (direction == "1E") return 1;
    if (direction == "1S") return 2;
    if (direction == "1W") return 3;
    else return INT_MAX;
}


string Manager::int2dir(const int& direction) const
{
    if (direction == 4) return "00";
    if (direction == 0) return "1N";
    if (direction == 1) return "1E";
    if (direction == 2) return "1S";
    if (direction == 3) return "1W";
    else return "ER";
}


void Manager::add_arr(int (&arr1)[n_dir], const int (&arr2)[n_dir]) const
{
    for (int i = 0; i < n_dir; ++i)
        arr1[i] += arr2[i];
}


int Manager::run_check(int (&arr)[n_dir]) const
{
    int sum = 0;
    for (int i = 0; i < n_dir; ++i) {
        if (arr[i] > 1) return 0;
        sum += arr[i];
    }
    #ifdef DEBUG
    assert((0 <= sum) && (sum <= 4));
    #endif
    if (sum > 0) return sum;
    else return 0;
}


/**********************************/
/*          Main Function         */
/**********************************/
int main(int argc, char** argv)
{   
	#ifdef DEBUG
	if (argc != 3) { /// argument count == 3 : ./manager <input_file> <output_file>
        cout << "Illegal number of arguments! Terminating program!" << endl; 
        return 1; }
    #endif
    
    Manager* M = new Manager();

    M->read_input(argv[1]);
    M->display_input(false);

    M->process();
    M->display_output(true);
    M->save_output(argv[2]);

    #ifdef DEBUG
    assert(M->valid_checker());
    #endif

    return 0;
}