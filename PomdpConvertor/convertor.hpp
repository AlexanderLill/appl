using namespace momdp;

namespace momdp {

    class Convertor {
        public:
            void convertToPomdpx(POMDP* problem, ofstream& pomdpxfile);
            void print_usage(const char* cmdName);

        private:
            void writeSparseVector(ostream& out, SparseVector& sv, int numStates);
            void writeSparseMatrix(ostream& out, SparseMatrix& sm, SparseMatrix& smtr, int action, char type, int numStates);
            void writeSparseMatrixReward(ostream& out, SparseMatrix& sm);
        };
}
