Graph = load 'graph' using PigStorage(',') as (u:int, v:int);
Graph = order Graph by u, v;
Graph = distinct Graph;
Graph_group = group Graph by u;
Graph_group_flattened = foreach Graph_group {
                            V = order Graph by $1;
                            V = V.$1; 
                            generate flatten(V) as V1, flatten(V) as V2, flatten(group) as V3;
                        };
Graph_group_flattened = filter Graph_group_flattened by V1 < V2;
Graph_group_flattened_back_join = join Graph_group_flattened by (V1,V2), Graph by (u,v);
Graph_triangles = foreach Graph_group_flattened_back_join generate Graph_group_flattened::V1 as v1, Graph_group_flattened::V2 as v2, Graph_group_flattened::V3 as v3;

dump Graph_triangles; 
