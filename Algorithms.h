#pragma once


class ConsuNode_Sol{
public:
	int path_cost = 0; // 完成该节点的带宽成本
	int next_sol=-1;
	vector<vector<int>> path_group;
	ConsuNode_Sol(int pathcost, vector<vector<int>> pathgroup) {
		this->path_cost = pathcost;
		this->path_group = pathgroup;

	}

};


class Algorithms
{
public:
	vector<vector<Link_Info>> G_Matrix_Origin;

	Algorithms(vector<vector<Link_Info>> &G_Matrix);
	~Algorithms();

	template<class _Cond_fun>
	vector<int> Dijkstra(const vector<vector<Link_Info>> &G_Matrix, \
		int u, int v, int demand, _Cond_fun Cond);

	vector<pair<int, int>>  max_capacity(vector<vector<int>> & input_data);
	
	vector<vector<int>> solver(vector<vector<Link_Info>> &G_Matrix,
		vector<pair<int, int>> Customers, vector<vector<int>> &input_data);


	vector<vector<int>>  Algorithms::solver_1(vector<vector<Link_Info>> &G_Matrix,
		vector<pair<int, int>> Customers, vector<vector<int>> &input_data);


	void Algorithms::update_G_Matrix(vector<vector<Link_Info>> &G_Matrix, vector<vector<int>> path_group, int actual_flow);

	void print_path(const vector<vector<int>> &path_group, string msg);
	void print_G_Matrix(const vector<vector<Link_Info>> &G_Matrix);
	vector<Link_Info> Access_G_M(const vector<vector<Link_Info>> &G_Matrix, int i, int j);

	int min_num_not_equal_x(const vector<int>& nums, int x);

	vector<int> get_best_path(vector<vector<int>> &path_group);
	void check(const vector<vector<Link_Info>> &G_Matrix);


	void roll_back_path(vector<vector<Link_Info>> &G_Matrix,
		const vector<vector<int>> &path_group);

	int update_Consumers_solution(vector<vector<ConsuNode_Sol> > &Consumers_solution,
		vector<vector<Link_Info>> &G_Matrix, int customer_id, vector<vector<int>> tmp_path_group,
		int total_cost);


	bool is_better_solution(vector<vector<ConsuNode_Sol>> &Consumers_solution,
		int customer_id, vector<vector<int>> tmp_path_group);

};

