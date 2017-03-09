#include "stdafx.h"
#include "Algorithms.h"

Algorithms::Algorithms(vector<vector<Link_Info>> &G_Matrix)
{
	this->G_Matrix_Origin = G_Matrix;

}
Algorithms::~Algorithms()
{

}
template<class _Cond_fun>
vector<int> Algorithms::Dijkstra(const vector<vector<Link_Info>> &G_Matrix, 
	int u, int v, int demand, _Cond_fun Cond){
     // find the shortest path from departure to destination
	// 返回的路径要么满足流量要求，此时u->v 的路径中肯定包含其他节点;
	// 如果返回的路径中不包含其他节点，即 ->v ,那么可能不满足流量要求
	// 最后一个参数为控制条件
	// return : path ～{u->...->v,(maxmal flow alowed),(min rent cost),(actual flow) }, the minmal length of
	//          path should be 4  
	//
	int Nodes = G_Matrix.size(),x=u;

	vector<bool> selected(Nodes);
	vector<int> path_pre(Nodes);

	vector<Link_Info> D(Nodes, Link_Info(0,INF));
	for (int i = 0; i < Nodes; i++){
		selected[i] = false;
		D[i] = G_Matrix[u][i];
		if (D[i].unit_rent_cost < INF){ path_pre[i] = u; }
		else  path_pre[i] = -1; //表示当前还不能到达此点
	}
	selected[u] = true;
	for (int i = 0; i < Nodes; i++){
		int min = INF;
		for (int w = 0; w < Nodes; w++){
			if (!selected[w] && D[w].unit_rent_cost < min && Cond(demand, D[w].capacity)){//&& demand <= D[w].capacity
				min = D[w].unit_rent_cost; x = w;
			}
		}
		selected[x] = true;
		for (int s = 0; s < Nodes; s++){
			if (!selected[s] && min + G_Matrix[x][s].unit_rent_cost < D[s].unit_rent_cost){
				D[s].unit_rent_cost = min + G_Matrix[x][s].unit_rent_cost;
				if (x == 44||s==43){
					int stop = 1;
				}
				D[s].capacity = this->min_num_not_equal_x({ D[s].capacity, D[x].capacity, G_Matrix[x][s].capacity }, 0);
				path_pre[s] = x;
			}
		} 
	}
	vector<int> path; path.push_back(v);
	//cout << D[v].unit_rent_cost << endl;
	for (int pre = v; path_pre[pre] != -1; pre = path_pre[pre], path.insert(path.begin(), pre));
	
	path.push_back(D[v].capacity);        //maxmal flow alowed
	path.push_back(D[v].unit_rent_cost); 
	path.push_back(std::min(demand, D[v].capacity));


	return path;//the last three elements are max flow alowed  in this path , total cost and actual flow
	
}

int Algorithms::min_num_not_equal_x(const vector<int>& nums, int x){
	// find the minmal number which is not equal to x(=0 used)
	int result = INF;
	for (auto num : nums){
		if (num != x && num < result){
			result = num;
		}
	}
	return result;
}

vector<pair<int, int>>  Algorithms::max_capacity(vector<vector<int>> & input_data)
{   
	int Node_nums = input_data[0][0], link_nums = input_data[0][1];
	int max_capacity;
	vector<pair<int, int>> Capacities(Node_nums, pair<int,int>(0,0));
	for (int i = 4; i < 4 + link_nums;++i){//statistic capacity of flow of each node
		Capacities[input_data[i][0]].first = input_data[i][0];
		Capacities[input_data[i][1]].first = input_data[i][1];

		Capacities[input_data[i][0] ].second += input_data[i][min_rent_cost_id];
		Capacities[input_data[i][1]].second += input_data[i][min_rent_cost_id];
	}
	sort(Capacities.begin(), Capacities.end(),
		[](const pair<int, int> &x, const pair<int, int> &y)->int{return x.second > y.second; });
	
	return Capacities;
}
vector<vector<int>>  Algorithms:: solver(vector<vector<Link_Info>> &G_Matrix,
	vector<pair<int,int>> Customers, vector<vector<int>> &input_data)
{
	int maxmal_capacity_alowed = 0, minmal_total_rent_cost = 0,actual_satisfied_flow=0;
	int consumer_netnode_id, consumer_netnode_demand;
	int remain_demand = 0;
	int total_cost = 0;
	vector<vector<int>> result; result.push_back(vector<int>(0));
	vector<int> tmp_path;
	vector<vector<int>> path_group;
	vector<vector<Link_Info>> G_Matrix_copy = G_Matrix;

	vector<pair<int, int>> Customers_Backup = Customers;
	
	sort(Customers.begin(), Customers.end(),
		[](const pair<int, int> &x, const pair<int, int> &y)->int{return x.second > y.second; });

	vector<pair<int,int>> capacity_sorted=this->max_capacity(input_data);
	vector<int> Server;

	for (auto cus : Customers) remain_demand += cus.second;
	//template<class _Cond_fun>
	//vector<_Cond_fun> my_rules{ [](int demand, int capacity)->int{return demand <= capacity; },
	//	                        [](int demand, int capacity)->bool {return capacity>0; } 
	//                           }; //按流量选取路径规则
	for (int i = 0, capa_sum = 0; i < capacity_sorted.size() && capa_sum < remain_demand + 2000;
		capa_sum += capacity_sorted[i].second, Server.push_back(i), i++);
		cout <<"numbers of server:"<< Server.size()<<endl;
		do{
			for (int i = 0; i < Customers.size();++i){
				consumer_netnode_id = Customers[i].first;
				consumer_netnode_demand = Customers[i].second;
				path_group.clear();
				if (consumer_netnode_demand != 0){
					for (auto ser_id : Server){
						//1 能否完全满足流量需求
						path_group.push_back(this->Dijkstra(G_Matrix_copy, capacity_sorted[ser_id].first, 
							consumer_netnode_id,consumer_netnode_demand, //流量需求
							[](int demand, int capacity)->int{return demand <= capacity; }));
					}
					tmp_path = this->get_best_path(path_group);
					maxmal_capacity_alowed = *(tmp_path.end() - max_flow_alowed_id);
					minmal_total_rent_cost = *(tmp_path.end() - min_rent_cost_id);
					//min_rent_cost_id 如果没找合适的路径则松弛条件
					if (minmal_total_rent_cost >= INF){ //find shortest path sucessfully
						path_group.clear();
					    for (auto ser_id : Server)
					     	path_group.push_back(this->Dijkstra(G_Matrix_copy, capacity_sorted[ser_id].first, 
							consumer_netnode_id, consumer_netnode_demand,
						    [](int demand, int capacity)->bool {return capacity > 0; })); 
					    tmp_path = this->get_best_path(path_group);
						maxmal_capacity_alowed = *(tmp_path.end() - max_flow_alowed_id);
						minmal_total_rent_cost = *(tmp_path.end() - min_rent_cost_id);
					}
					if (minmal_total_rent_cost < INF){
						this->print_path({ tmp_path }, "\n\n");
						actual_satisfied_flow = *(tmp_path.end() - 1); 
						total_cost += (minmal_total_rent_cost*actual_satisfied_flow);
						cout << "current total cost:" << total_cost << endl;
						remain_demand -= actual_satisfied_flow;
						Customers[i].second = Customers[i].second - actual_satisfied_flow;
						this->update_G_Matrix(G_Matrix_copy, {tmp_path}, actual_satisfied_flow);
						result.push_back(tmp_path);
					}
				}//if
			}//for
		} while (remain_demand != 0);//for 1
	// iterate netnode
	return result;
}

vector<vector<int>>  Algorithms::solver_1(vector<vector<Link_Info>> &G_Matrix,
	vector<pair<int, int>> Customers, vector<vector<int>> &input_data)
{   
	int maxmal_capacity_alowed = 0, minmal_total_rent_cost = 0, actual_satisfied_flow = 0;
	int consumer_netnode_id, consumer_netnode_demand;
	int remain_demand = 0,solu_cost=0;
	int total_cost = 0;
	int Server_rent_cost = input_data[2][0];
	vector<vector<int>> result; result.push_back(vector<int>(0));
	vector<int> tmp_path;
	vector<vector<int>> path_group;
	vector<vector<Link_Info>> G_Matrix_copy = G_Matrix;
	this->G_Matrix_Origin = G_Matrix;

	vector<pair<int, int>> Customers_Backup = Customers;
	sort(Customers.begin(), Customers.end(),
		[](const pair<int, int> &x, const pair<int, int> &y)->int{return x.second > y.second; });

	vector<pair<int, int>> capacity_sorted = this->max_capacity(input_data);
	vector<int> Server;

	vector<vector<ConsuNode_Sol>> Consumers_solution(Customers.size(),
		vector<ConsuNode_Sol>(capacity_sorted.size() + 1, ConsuNode_Sol(Server_rent_cost,{})));
	
	
	int best_pos = capacity_sorted.size();

    // The last position in Consumers_solution stores the best solution 
	for (auto cus : Customers) remain_demand += cus.second;
	//template<class _Cond_fun>
	//vector<_Cond_fun> my_rules{ [](int demand, int capacity)->int{return demand <= capacity; },
	//	                        [](int demand, int capacity)->bool {return capacity>0; } 
	//                           }; //按流量选取路径规则
	for (int m = 0; m < capacity_sorted.size(); m++){
		int ser_id=capacity_sorted[m].first;
		for (int i = 0; i < Customers.size(); ++i){
			if (Customers[i].second == 0)continue;
			consumer_netnode_id = Customers[i].first;
			consumer_netnode_demand = Customers[i].second;

			this->roll_back_path(G_Matrix_copy, Consumers_solution[i][best_pos].path_group);

			tmp_path=this->Dijkstra(G_Matrix_copy,ser_id,
				consumer_netnode_id, consumer_netnode_demand, //流量需求
				[](int demand, int capacity)->int{return demand <= capacity; }) ;

			maxmal_capacity_alowed = *(tmp_path.end() - max_flow_alowed_id);
			minmal_total_rent_cost = *(tmp_path.end() - min_rent_cost_id);
			actual_satisfied_flow = *(tmp_path.end() - actual_flow_id);
			solu_cost = actual_satisfied_flow*minmal_total_rent_cost;
			vector<vector<int>> tmp_path_group(1, tmp_path);
			//if (minmal_total_rent_cost < INF && solu_cost< Server_rent_cost )
			if (is_better_solution(Consumers_solution,i,{ tmp_path }))
			{   //找到可行解
				this->print_path({ tmp_path }, "\n\n");
				this->print_path( Consumers_solution[i][best_pos].path_group,"->deleted!\n");

				total_cost += solu_cost;
				cout << "current total cost:" << total_cost << endl;
				remain_demand -= actual_satisfied_flow;
				Customers[i].second = Customers[i].second - actual_satisfied_flow;
				this->update_G_Matrix(G_Matrix_copy, { tmp_path }, actual_satisfied_flow);
				Consumers_solution[i][ser_id].path_cost = solu_cost;
				Consumers_solution[i][ser_id].path_group.push_back(tmp_path);
				result.push_back(tmp_path);
			}else{
				this->update_G_Matrix(G_Matrix_copy,  Consumers_solution[i][best_pos].path_group , actual_satisfied_flow);

			}
		}
	}
	// iterate netnode
	this->check(G_Matrix_copy);
	return result;
}
bool Algorithms::is_better_solution(vector<vector<ConsuNode_Sol>> &Consumers_solution,
	int customer_id, vector<vector<int>> tmp_path_group)
{
	int best_pos = Consumers_solution[0].size()-1 ;
	if (tmp_path_group.size() == 0){    //tmp_path is not a valid path 
		return 0;
	}else{
		int minmal_total_rent_cost = 0;
		for (auto tmp_path : tmp_path_group){ //计算新的路径满足此节点所花费成本
			minmal_total_rent_cost += *(tmp_path.end() - min_rent_cost_id);
		}
		if (minmal_total_rent_cost < Consumers_solution[customer_id][best_pos].path_cost){
			return 1;
		}else{ 
			return 0; 
		}
	}
}
int Algorithms::update_Consumers_solution(vector<vector<ConsuNode_Sol> > &Consumers_solution,
	vector<vector<Link_Info>> &G_Matrix,int customer_id, vector<vector<int>> tmp_path_group,
	int total_cost)

{
	int best_pos = Consumers_solution[0].size() + cons_best_solu_offset;
	if (tmp_path_group.size()==0){//tmp_path is not a valid path 
		return 0;
	    
	}else{
		int minmal_total_rent_cost=0;
		for (auto tmp_path : tmp_path_group){ //计算新的路径满足此节点所花费成本
			minmal_total_rent_cost += *(tmp_path.end() - min_rent_cost_id);
		}
		if (minmal_total_rent_cost < Consumers_solution[customer_id][best_pos].path_cost){
			  
			this->roll_back_path(G_Matrix, Consumers_solution[customer_id][best_pos].path_group);
	        
			Consumers_solution[customer_id][best_pos].path_cost = minmal_total_rent_cost;
			Consumers_solution[customer_id][best_pos].path_group = tmp_path_group;
		    return 1;
		}
	
	}
}

//在一组路径中选择最好的一条路径，按照租用费用最低选择
vector<int> Algorithms::get_best_path(vector<vector<int>> &path_group){
	int min_unit_rent_cost = INF; 
	vector<int> result = path_group[0];
	for (int i = 0; i < path_group.size(); ++i){
		auto p = path_group[i].end() - min_rent_cost_id; //倒数第二列是最小成本
		if (*(p) < min_unit_rent_cost){
			result = path_group[i];
		}
	}
	return result;
}
// 根据一条路径更新图的邻接矩阵，包括每条链路的可用带宽
void  Algorithms::update_G_Matrix( vector<vector<Link_Info>> &G_Matrix,
	vector<vector<int>> path_group,int actual_flow){
	// 功能：1 按路径更新图，中的权值，如果某条路径被选择，则对应的capacity 被减去；
	// 
	for (auto path : path_group){
		for (int i = 1; i < path.size() - max_flow_alowed_id; ++i){
			int pre = path[i - 1], cur = path[i];
			G_Matrix[pre][cur].capacity -= actual_flow;

			if (G_Matrix[pre][cur].capacity == 0){
				G_Matrix[pre][cur].unit_rent_cost = INF;
			}
			if (G_Matrix[pre][cur].capacity < 0)
			{
				cout << "Exception!" << endl;
			}
		}
	}
}
void  Algorithms::roll_back_path(vector<vector<Link_Info>> &G_Matrix, 
	const vector<vector<int>> &path_group)
{//  将被删除路径上流量归还给图
	for (auto path : path_group)
	  for (int i = 1; i < path.size() - max_flow_alowed_id ; ++i){
		int pre = path[i - 1], cur = path[i];
		G_Matrix[pre][cur].capacity += *(path.end() - actual_flow_id  );

		if (G_Matrix[pre][cur].capacity > 0){
			G_Matrix[pre][cur].unit_rent_cost = this->G_Matrix_Origin[pre][cur].unit_rent_cost;
		}
		if (G_Matrix[pre][cur].capacity<0)
		{
			cout << "Exception!" << endl;
		}
	}
}

//打印一条路径
void Algorithms::print_path(const vector<vector<int>> &path_group,string msg){
	int i=0;
	for (auto path : path_group){

		for (i = 0; i < path.size() - max_flow_alowed_id - 1; ++i){
			cout << path[i] << "-->";
		}
		cout << path[i] << ",maxmal capacity alowed:" << path[i + 1] << ",actual flow:" <<
			path[i + 3] << ",cost of path:" << path[i + min_rent_cost_id] << msg;
	}
	
}
void Algorithms::print_G_Matrix(const vector<vector<Link_Info>> &G_Matrix){
	for (int i = 0; i < G_Matrix.size(); ++i){
		for (int j = 0; j < G_Matrix[i].size(); ++j){
			cout << "(" << G_Matrix[i][j].capacity << "," << G_Matrix[i][j].unit_rent_cost << ")" << "-";
		}
		cout << endl;
	}
}

vector<Link_Info> Algorithms::Access_G_M(const vector<vector<Link_Info>> &G_Matrix, int u, int v){
	vector<Link_Info> tmp;
	if (u == -1){
		for (int i = 0; i < G_Matrix.size(); tmp.push_back(G_Matrix[i][v]), ++i);
	}else if (v==-1)
	{   
		for (int j = 0; j < G_Matrix.size(); tmp.push_back(G_Matrix[u][j]), ++j);
	}else{
		tmp.push_back( G_Matrix[u][v]);
	}
	return  tmp ;
}

void Algorithms::check(const vector<vector<Link_Info>> &G_Matrix){

	for (int i = 0; i < G_Matrix.size(); ++i){
		for (int j = 0; j < G_Matrix.size(); ++j){
			if (G_Matrix[i][j].capacity < 0){
				cout << "G_Matrix[" << i << "][" << j << "].capacity=" << G_Matrix[i][j].capacity << endl;
			}
		}
	}
}