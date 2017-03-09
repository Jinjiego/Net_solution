import random
import networkx as nx
import matplotlib.pyplot as plt
def read_csv(fn):
     res=[]
     for line in open(fn).readlines():
          line=line.strip().split(' ')
          res.append(line)
     return res
def draw():
     pass
def parse(data_input,ID):
     G = nx.Graph()
     link_nums=int(data_input[0][1])
     Nodes_nums=int(data_input[0][0])
     customers=data_input[link_nums+5:]
     cus_node=[int(x[1]) for x in  customers]
     Nodes_color=['y']*Nodes_nums
     for rn in cus_node: Nodes_color[rn]='r'
     color_tb=['r','y','b','g','m']
     edges_color=[]
     G.add_nodes_from(range(Nodes_nums), node_color=Nodes_color)
     edge_labels={}
     for L in data_input[4:link_nums+4]:
        if len(L)==4:
           line=[int(x) for x in L]
           clr=color_tb[random.randint(0,len(color_tb)-1)]
           edges_color.append(  clr )
           edge_labels[tuple([line[0],line[1]] ) ]="("+L[2]+","+L[3]+")"
           G.add_edge(line[0],line[1],weight=line[3])
     pos = nx.fruchterman_reingold_layout(G) #spectral_layout,shell_layout,spring_layout,
     plt.figure(2)
     nx.draw(G,pos,node_color=Nodes_color,edge_color=edges_color, with_labels=True)
     nx.draw_networkx_edge_labels(G,pos,edge_labels=edge_labels,font_size=8)#,node_color=Nodes_color,edge_color=edges_color, with_labels=True)
     plt.savefig(str(ID)+".eps",format='eps')
     plt.show()
     sp=nx.shortest_path(G, source=0, target=20)
     print(sp)
