#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include "libraries/Matrix.cpp"

using namespace std;

struct settings{
    string block_symbol = "@";
    string non_block_symbol = "`";
    size_t grid_space = 1;
};

struct stats{
    size_t high_score = 0;
    double avg_score = 0;
    size_t games_played = 0;
    size_t shapes_placed = 0;
};

void display_grid(Matrix<bool>& Grid, size_t space = 0, string block = "@", string non_block = "`"){
    cout << "  ";
    for (size_t k = 0; k < space; k++){
        cout << "   ";
    }
    
    for (size_t i = 1; i <= Grid.get_cols(); i++){
        cout << i;
        if (i<10) cout << " ";
        for (size_t j = 0; j < space; j++){
            cout << "   ";
        }
    }
    cout << endl;
    for (size_t i = 0; i < space; i++){
        cout << endl;
    }
    
    for (size_t i = 0; i < Grid.get_rows(); i++){
        cout << i+1;
        if (i < 9) cout << " ";
        for (size_t k = 0; k < space; k++){
            cout << "   ";
        }

        for (size_t j = 0; j < Grid.get_cols(); j++){
            if (Grid[i][j]) cout << block[0] << " ";
            else cout << non_block[0] << " ";
            
            for (size_t k = 0; k < space; k++){
                cout << "   ";
            }
        }
        cout << endl;

        for (size_t k = 0; k < space; k++){
            cout << endl;
        }
    }
}

void display_score(size_t& score, size_t& high_score){
    cout << "Score: " << score << "                    High score: " << high_score << endl << endl;
}

vector<Matrix<bool>> define_shapes_vector(){
    vector<Matrix<bool>> shapes;

    Matrix<bool> shape(1, 3, true);
    shapes.push_back(shape);
    
    shape.resize(1, 4);
    shape.fill(true);
    shapes.push_back(shape);
    
    shape.resize(1, 5);
    shape.fill(true);
    shapes.push_back(shape);
    
    shape.resize(2, 2);
    shape.fill(true);
    shapes.push_back(shape);
    
    shape.resize(2, 3);
    shape.fill(true);
    shapes.push_back(shape);
    
    shape.resize(3, 3);
    shape.fill(true);
    shapes.push_back(shape);
    
    shape.resize(2, 2);
    shape.fill(true);
    shape[0][1] = false;
    shapes.push_back(shape);
    
    shape.resize(2, 3);
    shape.fill(true);
    shape[0][0] = false;
    shape[0][2] = false;
    shapes.push_back(shape);
    
    shape.resize(2, 3);
    shape.fill(true);
    shape[0][0] = false;
    shape[0][1] = false;
    shapes.push_back(shape);
    
    shape.resize(2, 3);
    shape.fill(true);
    shape[0][1] = false;
    shape[0][2] = false;
    shapes.push_back(shape);
    
    shape.resize(3, 3);
    shape.fill(true);
    shape[0][1] = false;
    shape[0][2] = false;
    shape[1][1] = false;
    shape[1][2] = false;
    shapes.push_back(shape);
    
    shape.resize(2, 2);
    shape.fill(true);
    shape[0][1] = false;
    shape[1][0] = false;
    shapes.push_back(shape);
    
    shape.resize(3, 3);
    shape.fill(false);
    shape[0][0] = true;
    shape[1][1] = true;
    shape[2][2] = true;
    shapes.push_back(shape);
    
    shape.resize(2, 3);
    shape.fill(true);
    shape[0][0] = false;
    shape[1][2] = false;
    shapes.push_back(shape);
    
    shape.resize(2, 3);
    shape.fill(true);
    shape[0][2] = false;
    shape[1][0] = false;
    shapes.push_back(shape);
    
    return shapes;
}

Matrix<bool> rotate_shape(Matrix<bool>& shape, int angle){
    angle %= 360;
    if (angle < 0) angle += 360;
    angle /= 90;

    int rows = shape.get_rows();
    int cols = shape.get_cols();

    Matrix<bool> rotated_shape = shape;

    if (angle == 1){
        rotated_shape.resize(shape.get_cols(), shape.get_rows());
        for (size_t i = 0; i < rotated_shape.get_rows(); i++){
            for (size_t j = 0; j < rotated_shape.get_cols(); j++){
                rotated_shape[i][j] = shape[rows-j-1][i];
            }
        }
    }
    else if (angle == 2){
        for (size_t i = 0; i < rotated_shape.get_rows(); i++){
            for (size_t j = 0; j < rotated_shape.get_cols(); j++){
                rotated_shape[i][j] = shape[rows-i-1][cols-j-1];
            }
        }
    }
    else if (angle == 3){
        rotated_shape.resize(shape.get_cols(), shape.get_rows());
        for (size_t i = 0; i < rotated_shape.get_rows(); i++){
            for (size_t j = 0; j < rotated_shape.get_cols(); j++){
                rotated_shape[i][j] = shape[j][cols-i-1];
            }
        }
    }

    return rotated_shape;
}

bool place_piece(Matrix<bool>& Grid, Matrix<bool>& shape, size_t row, size_t col){
    if ((row + shape.get_rows() > Grid.get_rows()) || (col + shape.get_cols() > Grid.get_cols())) return false;

    for (size_t i = row; i < shape.get_rows()+row; i++){
        for (size_t j = col; j < shape.get_cols()+col; j++){
            if (Grid[i][j] && shape[i-row][j-col]) return false;
        }
    }

    for (size_t i = row; i < shape.get_rows()+row; i++){
        for (size_t j = col; j < shape.get_cols()+col; j++){
            Grid[i][j] = Grid[i][j] || shape[i-row][j-col];
        }
    }

    return true;
}

vector<size_t> clear_lines(Matrix<bool>& Grid){  //Returns number of rows and cols cleared
    vector<size_t> rows_cols_cleared;
    rows_cols_cleared.push_back(0);
    rows_cols_cleared.push_back(0);

    vector<size_t> rows_to_remove;
    vector<size_t> cols_to_remove;

    for (size_t i = 0; i < Grid.get_rows(); i++){
        bool isFilled = true;
        for (size_t j = 0; j < Grid.get_cols(); j++){
            if (!Grid[i][j]){
                isFilled = false;
                break;
            }
        }
        if (isFilled){
            rows_to_remove.push_back(i);
            rows_cols_cleared[0]++;
        }
    }

    for (size_t i = 0; i < Grid.get_cols(); i++){
        bool isFilled = true;
        for (size_t j = 0; j < Grid.get_rows(); j++){
            if (!Grid[j][i]){
                isFilled = false;
                break;
            }
        }
        if (isFilled){
            cols_to_remove.push_back(i);
            rows_cols_cleared[1]++;
        }
    }

    for (size_t i = 0; i < rows_to_remove.size(); i++){
        for (size_t j = 0; j < Grid.get_cols(); j++){
            Grid[rows_to_remove[i]][j] = false;
        }
    }

    for (size_t i = 0; i < cols_to_remove.size(); i++){
        for (size_t j = 0; j < Grid.get_rows(); j++){
            Grid[j][cols_to_remove[i]] = false;
        }
    }

    return rows_cols_cleared;
}

bool is_playable(Matrix<bool>& Grid, vector<Matrix<bool>>& shapes){
    for (size_t i = 0; i < shapes.size(); i++){
        for (size_t j = 0; j <= Grid.get_rows()-shapes[i].get_rows(); j++){
            for (size_t k = 0; k <= Grid.get_cols()-shapes[i].get_cols(); k++){
                bool can_be_placed = true;
                for (size_t l = 0; l < shapes[i].get_rows(); l++){
                    for (size_t m = 0; m < shapes[i].get_cols(); m++){
                        if (Grid[l+j][m+k] && shapes[i][l][m]){
                            can_be_placed = false;
                            break;
                        }
                    }
                    if (!can_be_placed) break;
                }
                if (can_be_placed) return true;
            }
        }
    }

    return false;
}

vector<Matrix<bool>> get_random_shapes(vector<Matrix<bool>> shapes, size_t no_of_shapes = 3){
    vector<Matrix<bool>> random_shapes;
    
    size_t min = 0;
    size_t max = shapes.size()-1;

    for (size_t i = 0; i < no_of_shapes; i++){
        Matrix<bool> shape = shapes[Matrix<size_t>::generate_random_number(min, max)];
        shape = rotate_shape(shape, Matrix<size_t>::generate_random_number(0, 3) * 90);
        random_shapes.push_back(shape);
    }

    return random_shapes;
}

void display_shapes(vector<Matrix<bool>>& shapes){
    size_t max_height = 0;
    for (size_t i = 0 ; i < shapes.size(); i++){
        if (shapes[i].get_rows() > max_height) max_height = shapes[i].get_rows();
    }

    for (size_t i = 0; i < max_height; i++){
        for (size_t j = 0; j < shapes.size(); j++){
            cout << " ";
            for (size_t k = 0; k < 5; k++){
                if ((i < shapes[j].get_rows()) && (k < shapes[j].get_cols())){
                    if (shapes[j][i][k]) cout << "* ";
                    else cout << "  ";
                }
                else{
                    cout << "  ";
                }
            }
            cout << " ";
        }
        cout << endl;
    }
    cout << "      ";
    for (size_t i = 1; i <= shapes.size(); i++){
        cout << i << "          ";
    }
    cout << endl << endl;
}

void save_setings(settings s){
    ofstream file("game data/settings.csv");
    if (!file.is_open()){
        std::cerr << "failed to open file for writing.\n";
        return;
    }

    file << s.block_symbol + ",";
    file << s.non_block_symbol + ",";
    file << to_string(s.grid_space) + ",";

    file.close();

    return;
}

settings load_settings(){
    settings s;

    ifstream file("game data/settings.csv");
    string line;

    if (getline(file, line)){
        stringstream ss(line);
        string col1, col2, col3;

        getline(ss, col1, ',');
        getline(ss, col2, ',');
        getline(ss, col3, ',');

        s.block_symbol = col1;
        s.non_block_symbol = col2;
        s.grid_space = stoi(col3);
    }

    file.close();

    return s;
}

void show_settings(){
    settings s = load_settings();
    string user_input;

    while (true){
        cout << "1.Block symbol: " + s.block_symbol << endl;
        cout << "2.Non block symbol: " + s.non_block_symbol << endl;
        cout << "3.Grid space: " + to_string(s.grid_space) << endl;
        cout << "4.Reset to default" << endl;
        cout << "5.Back(save settings)" << endl;
        cout << "Enter input: ";

        cin >> user_input;

        cout << endl << endl;
        if (user_input == "1"){
            string block_Symbol;

            while(true){
                cout << "Enter block symbol (or type NA): ";
                cin >> block_Symbol;

                if (block_Symbol == "NA" || block_Symbol == "Na" || block_Symbol == "nA" || block_Symbol == "na"){
                    break;
                }
                else if (block_Symbol.length() == 1){
                    s.block_symbol = block_Symbol;
                    break;
                }
                else{
                    cout << "Invalid input!" << endl;
                }
            }
        }
        else if (user_input == "2"){
            string non_block_Symbol;

            while(true){
                cout << "Enter non block symbol (or type NA): ";
                cin >> non_block_Symbol;

                if (non_block_Symbol == "NA" || non_block_Symbol == "Na" || non_block_Symbol == "nA" || non_block_Symbol == "na"){
                    break;
                }
                else if (non_block_Symbol.length() == 1){
                    s.non_block_symbol = non_block_Symbol;
                    break;
                }
                else{
                    cout << "Invalid input!" << endl;
                }
            }
        }
        else if (user_input == "3"){
            size_t grid_space;

            while(true){
                cout << "Enter grid size (or type 0): ";
                cin >> grid_space;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Enter a valid input!" << endl;
                } else {
                    s.grid_space = grid_space;
                    break;
                }
            }
        }
        else if (user_input == "4"){
            settings default_settings;
            default_settings.block_symbol = "@";
            default_settings.non_block_symbol = "`";
            default_settings.grid_space = 1;
            s = default_settings;
        }
        else if (user_input == "5"){
            save_setings(s);
            break;
        }
        else{
            cout << "Invalid input!" << endl;
        }
    }
}

void save_stats(stats s){
    ofstream file("game data/stats.csv");
    if (!file.is_open()){
        std::cerr << "failed to open file for writing.\n";
        return;
    }

    file << to_string(s.high_score) + ",";
    file << to_string(s.avg_score) + ",";
    file << to_string(s.games_played) + ",";
    file << to_string(s.shapes_placed) + ",";

    file.close();

    return;
}

stats load_stats(){
    stats s;

    ifstream file("game data/stats.csv");
    string line;

    if (getline(file, line)){
        stringstream ss(line);
        string col1, col2, col3, col4;

        getline(ss, col1, ',');
        getline(ss, col2, ',');
        getline(ss, col3, ',');
        getline(ss, col4, ',');

        s.high_score = stoi(col1);
        s.avg_score = stod(col2);
        s.games_played = stoi(col3);
        s.shapes_placed = stoi(col4);
    }

    file.close();

    return s;
}

void show_stats(){
    stats s = load_stats();
    string user_input;

    cout << "High score: " + to_string(s.high_score) << endl;
    cout << "Average score: " + to_string(s.avg_score) << endl;
    cout << "Games played: " + to_string(s.games_played) << endl;
    cout << "Shapes placed: " + to_string(s.shapes_placed) << endl;

    cout  << endl;

    while(true){
        cout << "1.Reset stats" << endl;
        cout << "2.Back" << endl;
        cout << "Enter input: ";

        cin >> user_input;

        cout << endl << endl;
        if (user_input == "1"){
            string confirm;

            while(true){
                cout << "Are you sure (y/n): ";
                cin >> confirm;

                if (confirm == "Y" || confirm == "y"){
                    stats default_stats;
                    default_stats.high_score = 0;
                    default_stats.avg_score = 0;
                    default_stats.games_played = 0;
                    default_stats.shapes_placed = 0;
                    save_stats(default_stats);
                    break;
                }
                else if(confirm == "N" || confirm == "n"){
                    break;
                }
                else{
                    cout << "Invalid input!" << endl;
                }
            }

            break;
        }
        else if (user_input == "2"){
            break;
        }
        else{
            cout << "Invalid input!" << endl;
        }
    }
}



void run_game(){
    settings sett = load_settings();
    stats stat = load_stats();

    Matrix<bool> Grid(8, 8, false);
    vector<Matrix<bool>> shapes = define_shapes_vector();
    size_t score = 0;
    size_t combo = 0;

    vector<Matrix<bool>> options;
    options = get_random_shapes(shapes, 3);

    while (true){
        cout << endl;
        display_grid(Grid, sett.grid_space, sett.block_symbol, sett.non_block_symbol);
        display_score(score, stat.high_score);
        display_shapes(options);
        
        size_t shape_no;
        while(true){
            cout << "Choose a shape: ";
            cin >> shape_no;
            if (cin.fail() || shape_no > options.size() || shape_no == 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter a valid input!" << endl;
            } else {
                break;
            }
        }
        
        size_t row_no;
        while(true){
            cout << "Choose a row: ";
            cin >> row_no;
            if (cin.fail() || row_no > Grid.get_rows() || row_no == 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter a valid input!" << endl;
            } else {
                break;
            }
        }
        
        size_t col_no;
        while(true){
            cout << "Choose a column: ";
            cin >> col_no;
            if (cin.fail() || col_no > Grid.get_cols() || col_no == 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Enter a valid input!" << endl;
            } else {
                break;
            }
        }
        
        if (!place_piece(Grid, options[shape_no-1], row_no-1, col_no-1)){
            cout << "Invalid move!" << endl;
            continue;
        }

        stat.shapes_placed++;
        
        for (size_t i = 0; i < options[shape_no-1].get_rows(); i++){
            for (size_t j = 0; j < options[shape_no-1].get_cols(); j++){
                if (options[shape_no-1][i][j]) score++;
            }
        }
        
        options.erase(options.begin() + shape_no-1);
        
        if (options.size() == 0){
            options = get_random_shapes(shapes, 3);
        }
        
        vector<size_t> rows_cols_cleared = clear_lines(Grid);
        
        size_t points = 0;
        points += Grid.get_rows() * rows_cols_cleared[0];
        points += Grid.get_cols() * rows_cols_cleared[1];
        points *= rows_cols_cleared[0] + rows_cols_cleared[1];
        
        if (points) combo++;
        else combo = 0;
        
        score += combo * points;
        
        if (!is_playable(Grid, options)){
            display_grid(Grid, sett.grid_space);
            display_score(score, stat.high_score);
            display_shapes(options);
            cout << "Game Over!" << endl;
            stat.games_played++;
            if (score > stat.high_score) stat.high_score = score;
            stat.avg_score = stat.avg_score + (score + stat.avg_score) / (stat.games_played);

            save_stats(stat);
            break;
        }
    }
}

void run_application(){
    string user_input;
    while (true){
        cout << "1.Start game" << endl;
        cout << "2.Settings" << endl;
        cout << "3.Stats" << endl;
        cout << "4.Quit" << endl;
        cout << "Enter input: ";

        cin >> user_input;

        cout << endl << endl;
        if (user_input == "1"){
            run_game();
        }
        else if (user_input == "2"){
            show_settings();
        }
        else if (user_input == "3"){
            show_stats();
        }
        else if (user_input == "4"){
            break;
        }
        else{
            cout << "Invalid input!" << endl;
        }
    }
}

int main(){
    run_application();
    
    return 0;
}