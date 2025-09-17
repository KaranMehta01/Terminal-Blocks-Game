#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>


template<typename T>
struct AllowType {
    static const bool allowed = false;
};

template<> struct AllowType<signed char>        { static const bool allowed = true; };
template<> struct AllowType<short>              { static const bool allowed = true; };
template<> struct AllowType<int>                { static const bool allowed = true; };
template<> struct AllowType<long>               { static const bool allowed = true; };
template<> struct AllowType<long long>          { static const bool allowed = true; };
template<> struct AllowType<unsigned char>      { static const bool allowed = true; };
template<> struct AllowType<unsigned short>     { static const bool allowed = true; };
template<> struct AllowType<unsigned>           { static const bool allowed = true; };
template<> struct AllowType<unsigned long>      { static const bool allowed = true; };
template<> struct AllowType<unsigned long long> { static const bool allowed = true; };
template<> struct AllowType<float>              { static const bool allowed = true; };
template<> struct AllowType<double>             { static const bool allowed = true; };
template<> struct AllowType<long double>        { static const bool allowed = true; };
template<> struct AllowType<bool>               { static const bool allowed = true; };


template <typename T>
class Matrix{
private:
    unsigned ROWS, COLS, SIZE;
    T* DATA;

public:
    // For subscript operator "[]"
    class RowProxy{
    private:
        T* row_data;
        unsigned cols;
    public:
        RowProxy(T* data, unsigned c) : row_data(data), cols(c) {}

        T& operator[](unsigned col){
            if (col >= cols) {
                throw std::runtime_error("Error: Invalid column index");
            }
            
            return row_data[col];
        }
        const T& operator[](unsigned col) const {
            if (col >= cols) {
                throw std::runtime_error("Error: Invalid column index");
            }

            return row_data[col];
        }
    };

    // static long double generate_random_number(long double min, long double max) {
    //     static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    //     std::uniform_real_distribution<long double> dist(min, max);
    //     return dist(rng);
    // }

    static T generate_random_number(T min, T max) {
        static std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        if constexpr (std::is_same<T, bool>::value) {
            std::bernoulli_distribution dist((min || max) ? 0.5 : 0.0);
            return dist(rng);
        } else if constexpr (std::is_floating_point<T>::value) {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(rng);
        } else if constexpr (std::is_integral<T>::value) {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(rng);
        } else {
            throw std::runtime_error("Random generation not supported for this type!");
        }
    }

    static void swap(unsigned& a, unsigned& b){
        unsigned temp = a;
        a = b;
        b = temp;
    }

    // Constructor
    Matrix(unsigned r = 1, unsigned c = 1, T value = T()){
        static_assert(AllowType<T>::allowed, "Error: This type is not supported in Matrix!");
        ROWS = r;
        COLS = c;
        SIZE = r*c;
        DATA = new T[SIZE]{};
        if (value != T()){
            for (unsigned i = 0 ; i < SIZE ; i++){
                DATA[i] = value;
            }
        }
    }
    
    // Copy Constructor
    Matrix(const Matrix& other) : ROWS(other.ROWS), COLS(other.COLS), SIZE(other.SIZE) {
        DATA = new T[SIZE];
        for (unsigned i = 0; i < SIZE; ++i) {
            DATA[i] = other.DATA[i];
        }
    }

    // Copy Assignment Operator
    Matrix& operator=(const Matrix& other) {
        if (this != &other) {
            delete[] DATA;
            ROWS = other.ROWS;
            COLS = other.COLS;
            SIZE = other.SIZE;
            DATA = new T[SIZE];
            for (unsigned i = 0; i < SIZE; ++i) {
                DATA[i] = other.DATA[i];
            }
        }
        return *this;
    }

    // Move Constructor
    Matrix(Matrix&& other) noexcept : ROWS(other.ROWS), COLS(other.COLS), SIZE(other.SIZE), DATA(other.DATA) {
        other.DATA = nullptr;
        other.ROWS = other.COLS = other.SIZE = 0;
    }

    // Move Assignment Operator
    Matrix& operator=(Matrix&& other) noexcept {
        if (this != &other) {
            delete[] DATA;
            DATA = other.DATA;
            ROWS = other.ROWS;
            COLS = other.COLS;
            SIZE = other.SIZE;
            other.DATA = nullptr;
            other.ROWS = other.COLS = other.SIZE = 0;
        }
        return *this;
    }
    
    // Destructor
    ~Matrix(){
        delete[] DATA;
    }

    unsigned get_rows() const { return ROWS; }
    unsigned get_cols() const { return COLS; }
    unsigned get_size() const { return SIZE; }

    T get(unsigned row, unsigned column) const {
        if (row >= ROWS || column >= COLS){
            throw std::runtime_error("Error: Invalid index!");
        }

        return DATA[row * COLS + column];
    }

    void set(unsigned row, unsigned column, T new_data){
        if (row >= ROWS || column >= COLS){
            throw std::runtime_error("Error: Invalid index!");
        }

        DATA[row * COLS + column] = new_data;
    }

    void resize(unsigned r, unsigned c){
        if (r == 0 || c == 0){
            throw std::runtime_error("Error: Can't resize the matrix to size 0!");
        }
        
        T* newData = new T[r * c]{};
        unsigned minRows = (r < ROWS) ? r : ROWS;
        unsigned minCols = (c < COLS) ? c : COLS;
        
        for (unsigned i = 0; i < minRows; ++i) {
            for (unsigned j = 0; j < minCols; ++j) {
                newData[i * c + j] = DATA[i * COLS + j];
            }
        }
        
        delete[] DATA;
        DATA = newData;
        ROWS = r;
        COLS = c;
        SIZE = r * c;
    }
    
    void display(unsigned char width = 6) const {
        for (unsigned i = 0 ; i < ROWS ; i++){
            std::cout << "| ";
            for (unsigned j = 0 ; j < COLS ; j++){
                std::cout << std::setw(width) << DATA[i * COLS + j] << " ";
            }
            std::cout << "|" << std::endl;
        }
    }

    template <unsigned N>
    void insert_data(T (&arr)[N]){
        unsigned minSize = (N < SIZE) ? N : SIZE;

        for (unsigned i = 0 ; i < minSize ; i++){
            DATA[i] = arr[i];
        }

        if (N > SIZE){
            std::cout << "Size of matrix is small, not all data was copied!" << std::endl;
        }
    }

    void add(Matrix& m){
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] += m.DATA[i];
        }
    }
    
    Matrix return_add(Matrix& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        Matrix<T> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] + m.DATA[i];
        }

        return M;
    }

    void add_scalar(T scalar){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] += scalar;
        }
    }
    
    Matrix return_add_scalar(T scalar){
        Matrix<T> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] + scalar;
        }

        return M;
    }

    void subtract(Matrix& m){
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] -= m.DATA[i];
        }
    }

    Matrix return_subtract(Matrix& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        Matrix<T> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] - m.DATA[i];
        }

        return M;
    }

    void multiply(Matrix& m) {
        if (COLS != m.ROWS) {
            throw std::runtime_error("Error: Can't multiply!");
        }

        *this = this->return_multiply(m);
    }

    Matrix return_multiply(const Matrix& m) const {
        if (COLS != m.ROWS){
            throw std::runtime_error("Error: Can't multiply!");
        }

        Matrix<T> M(ROWS, m.COLS);

        for (unsigned i = 0 ; i < ROWS ; i++){
            for (unsigned j = 0 ; j < m.COLS ; j++){
                T value = T();
                for (unsigned k = 0 ; k < COLS ; k++){
                    value += get(i, k) * m.get(k, j);
                }
                M.set(i, j, value);
            }
        }

        return M;
    }

    void transpose(){
        Matrix<T> M(COLS, ROWS);

        for (unsigned i = 0 ; i < COLS ; i++){
            for (unsigned j = 0 ; j < ROWS ; j++){
                M.set(i, j, get(j, i));
            }
        }

        resize(COLS, ROWS);

        for (unsigned i = 0 ; i < ROWS ; i++){
            for (unsigned j = 0 ; j < COLS ; j++){
                set(i, j, M.get(i, j));
            }
        }
    }

    Matrix return_transpose() const {
        Matrix<T> M(COLS, ROWS);

        for (unsigned i = 0 ; i < COLS ; i++){
            for (unsigned j = 0 ; j < ROWS ; j++){
                M.set(i, j, get(j, i));
            }
        }

        return M;
    }
    
    Matrix operator+(T scalar) const{
        Matrix<T> M(ROWS, COLS);
        for (unsigned i = 0 ; i < M.SIZE ; i++){
            M.DATA[i] = DATA[i] + scalar;
        }
        return M;
    }

    Matrix operator+(const Matrix& m) const {
        return return_add(m);
    }
    
    Matrix operator-(T scalar) const{
        Matrix<T> M(ROWS, COLS);
        for (unsigned i = 0 ; i < M.SIZE ; i++){
            M.DATA[i] = DATA[i] - scalar;
        }
        return M;
    }

    Matrix operator-(const Matrix& m) const {
        return return_subtract(m);
    }
    
    Matrix operator*(T scalar) const{
        Matrix<T> M(ROWS, COLS);
        for (unsigned i = 0 ; i < M.SIZE ; i++){
            M.DATA[i] = DATA[i] * scalar;
        }
        return M;
    }
    
    Matrix operator*(const Matrix& m) const {
        return return_multiply(m);
    }
    
    Matrix operator/(T scalar) const {
        if (scalar == 0) {
            throw std::runtime_error("Error: Division by zero!");
        }
        Matrix<T> M(ROWS, COLS);
        for (unsigned i = 0 ; i < M.SIZE ; i++){
            M.DATA[i] = DATA[i] / scalar;
        }
        return M;
    }

    Matrix& operator+=(T scalar){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] += scalar;
        }

        return *this;
    }

    Matrix& operator+=(const Matrix& m){
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] += m.DATA[i];
        }

        return *this;
    }
    
    Matrix& operator-=(T scalar){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] -= scalar;
        }

        return *this;
    }
    
    Matrix& operator-=(const Matrix& m){
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] -= m.DATA[i];
        }

        return *this;
    }
    
    Matrix& operator*=(const Matrix& m){
        if (COLS != m.ROWS){
            throw std::runtime_error("Error: Can't multiply!");
        }

        Matrix<T> M(ROWS, m.COLS);

        for (unsigned i = 0 ; i < ROWS ; i++){
            for (unsigned j = 0 ; j < m.COLS ; j++){
                T value = T();
                for (unsigned k = 0 ; k < COLS ; k++){
                    value += get(i, k) * m.get(k, j);
                }
                M.set(i, j, value);
            }
        }

        *this = M;

        return *this;
    }

    Matrix& operator*=(T scalar){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] *= scalar;
        }
        return *this;
    }
    
    Matrix& operator/=(T scalar){
        if (scalar == 0) {
            throw std::runtime_error("Error: Division by zero!");
        }
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] /= scalar;
        }
        return *this;
    }
    
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // This  will  be  implemented in future
    // Matrix& operator/=(const Matrix& m){}
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    Matrix operator-() const {
        Matrix<T> M(ROWS, COLS);
        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = -DATA[i];
        }
        return M;
    }
    
    Matrix operator+() const {
        Matrix<T> M(ROWS, COLS);
        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i];
        }
        return M;
    }
    
    bool is_equal_to(const Matrix& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            return false;
        }
        
        for (unsigned i = 0 ; i < SIZE ; i++){
            if (DATA[i] != m.DATA[i]){
                return false;
            }
        }
        
        return true;
    }
    
    bool is_not_equal_to(const Matrix& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            return true;
        }
        
        for (unsigned i = 0 ; i < SIZE ; i++){
            if (DATA[i] != m.DATA[i]){
                return true;
            }
        }
        
        return false;
    }
    
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // This may be implemented in future
    // bool operator!(const Matrix& m){}
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    Matrix<bool> operator<(const Matrix<T>& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }
        
        Matrix<bool> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] < m.DATA[i];
        }

        return M;
    }
    
    Matrix<bool> operator<(T scalar) const {        
        Matrix<bool> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] < scalar;
        }

        return M;
    }
    
    Matrix<bool> operator>(const Matrix<T>& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }
        
        Matrix<bool> M(ROWS, COLS);
        
        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] > m.DATA[i];
        }
        
        return M;
    }
    
    Matrix<bool> operator>(T scalar) const {        
        Matrix<bool> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] > scalar;
        }

        return M;
    }
    
    Matrix<bool> operator<=(const Matrix<T>& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        Matrix<bool> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] <= m.DATA[i];
        }

        return M;
    }
    
    Matrix<bool> operator<=(T scalar) const {        
        Matrix<bool> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] <= scalar;
        }

        return M;
    }
    
    Matrix<bool> operator>=(const Matrix<T>& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        Matrix<bool> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] >= m.DATA[i];
        }

        return M;
    }
    
    Matrix<bool> operator>=(T scalar) const {        
        Matrix<bool> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] >= scalar;
        }

        return M;
    }
    
    Matrix<bool> operator==(const Matrix<T>& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        Matrix<bool> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] == m.DATA[i];
        }

        return M;
    }
    
    Matrix<bool> operator==(T scalar) const {        
        Matrix<bool> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] == scalar;
        }

        return M;
    }
    
    Matrix<bool> operator!=(const Matrix<T>& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        Matrix<bool> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] != m.DATA[i];
        }

        return M;
    }
    
    Matrix<bool> operator!=(T scalar) const {        
        Matrix<bool> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] != scalar;
        }

        return M;
    }
    
    Matrix operator&(const Matrix<T>& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        Matrix<T> M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] & m.DATA[i];
        }

        return M;
    }
    
    Matrix operator&(T scalar) const {        
        Matrix M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] & scalar;
        }

        return M;
    }
    
    Matrix operator|(const Matrix<T>& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        Matrix M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] | m.DATA[i];
        }

        return M;
    }
    
    Matrix operator|(T scalar) const {        
        Matrix M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] | scalar;
        }

        return M;
    }
    
    Matrix operator^(const Matrix<T>& m) const {
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        Matrix M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] ^ m.DATA[i];
        }

        return M;
    }
    
    Matrix operator^(T scalar) const {        
        Matrix M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = DATA[i] ^ scalar;
        }

        return M;
    }
    
    Matrix operator~() const {
        Matrix M(ROWS, COLS);

        for (unsigned i = 0 ; i < SIZE ; i++){
            M.DATA[i] = ~DATA[i];
        }

        return M;
    }
    
    Matrix& operator&=(const Matrix<T>& m){
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] &= m.DATA[i];
        }

        return *this;
    }
    
    Matrix& operator&=(T scalar){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] &= scalar;
        }

        return *this;
    }

    Matrix& operator|=(const Matrix<T>& m){
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] |= m.DATA[i];
        }

        return *this;
    }
    
    Matrix& operator|=(T scalar){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] |= scalar;
        }

        return *this;
    }

    Matrix& operator^=(const Matrix<T>& m){
        if (ROWS != m.ROWS || COLS != m.COLS){
            throw std::runtime_error("Error: Size mismatch!");
        }

        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] ^= m.DATA[i];
        }

        return *this;
    }
    
    Matrix& operator^=(T scalar){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] ^= scalar;
        }

        return *this;
    }

    RowProxy operator[](unsigned row){
        if (row >= ROWS){
            throw std::runtime_error("Error: Invalid row index!");
        }

        return RowProxy(DATA + row * COLS, COLS);
    }

    const RowProxy operator[](unsigned row) const {
        if (row >= ROWS){
            throw std::runtime_error("Error: Invalid row index!");
        }

        return RowProxy(DATA + row * COLS, COLS);
    }

    T& operator()(unsigned row, unsigned col){
        if (row >= ROWS || col >= COLS) {
            throw std::runtime_error("Error: Invalid index!");
        }

        return DATA[row * COLS + col];
    }

    const T& operator()(unsigned row, unsigned col) const {
        if (row >= ROWS || col >= COLS) {
            throw std::runtime_error("Error: Invalid index!");
        }

        return DATA[row * COLS + col];
    }

    friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& m){
        unsigned rows = m.get_rows(), cols = m.get_cols();
        for (unsigned i = 0 ; i < rows ; i++){
            os << "| ";
            for (unsigned j = 0 ; j < cols ; j++){
                os << std::setw(6) << m.DATA[i * cols + j] << " ";
            }
            os << "|";
            if (i < rows - 1){ 
                os << std::endl;
            }
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Matrix<T>& m) {
        unsigned rows = m.get_rows(), cols = m.get_cols();
        for (unsigned i = 0; i < rows; i++) {
            for (unsigned j = 0; j < cols; j++) {
                T value;
                is >> value;
                if (is.fail()) {
                    throw std::runtime_error("Error: Invalid input!");
                }
                m.DATA[i * cols + j] = value;
            }
        }
        return is;
    }

    static Matrix<T> identity_matrix(unsigned size){
        Matrix<T> M(size, size);
        for (unsigned i = 0 ; i < size ; i++){
            M.DATA[i * (size + 1)] = 1;
        }
        return M;
    }

    static Matrix<T> zeros_matrix(unsigned rows, unsigned cols){
        Matrix<T> M(rows, cols);
        return M;
    }
    
    static Matrix<T> ones_matrix(unsigned rows, unsigned cols){
        Matrix<T> M(rows, cols, 1);
        return M;
    }

    static Matrix<T> filled_matrix(unsigned rows, unsigned cols, T scalar = T()){
        Matrix<T> M(rows, cols, scalar);
        return M;
    }

    static Matrix<T> random_matrix(unsigned rows, unsigned cols, T min = 0, T max = 1){
        Matrix<T> M(rows, cols);
        
        for (unsigned i = 0 ; i < M.SIZE ; i++){
            M.DATA[i] = generate_random_number(min, max);
        }

        return M;
    }
    
    void identity(){
        unsigned n = (ROWS < COLS)? ROWS : COLS;
        resize(n, n);

        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] = 0;
        }

        for (unsigned i = 0 ; i < n ; i++){
            DATA[i * (n + 1)] = 1;
        }
    }
    
    void zeros(){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] = 0;
        }
    }
    
    void ones(){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] = 1;
        }
    }
    
    void fill(T scalar = T()){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] = scalar;
        }
    }
    
    void fill_random(T min = 0, T max = 1){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] = generate_random_number(min, max);
        }
    }

    Matrix submatrix(unsigned start_row, unsigned start_col, unsigned end_row, unsigned end_col) const {
        if (start_row > end_row){
            swap(start_row, end_row);
        }
        if (start_col > end_col){
            swap(start_col, end_col);
        }

        if (start_row >= ROWS || start_col >= COLS || end_row > ROWS || end_col > COLS){
            throw std::runtime_error("Error: Invalid index!");
        }

        Matrix<T> M(end_row-start_row, end_col-start_col);

        for (unsigned i = start_row ; i < end_row ; i++){
            for (unsigned j = start_col ; j < end_col ; j++){
                M.set(i-start_row, j-start_col, get(i, j));
            }
        }

        return M;
    }

    template <typename Func>
    Matrix apply(Func func) const {
        Matrix<T> result(ROWS, COLS);
        for (unsigned i = 0 ; i < SIZE ; i++){
            result.DATA[i] = func(DATA[i]);
        }
        return result;
    }
    
    template <typename Func>
    void apply(Func func){
        for (unsigned i = 0 ; i < SIZE ; i++){
            DATA[i] = func(DATA[i]);
        }
    }
};

// int main(){
//     int a[] = {2, 0, 0, 0, 2, 0, 0, 0, 2};
//     int b[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

//     Matrix<int> m1(3, 3);
//     Matrix<int> m2(4, 3);

//     m1.insert_data(a, 9);
//     m2.insert_data(b, 12);

//     m1.display(1);
//     m2.display(2);
    
//     m2 *= m1;
//     std::cout << m2 << std::endl;
//     std::cout << "Hello";

//     return 0;
// }
