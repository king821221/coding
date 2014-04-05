Graph = load 'graph' using PigStorage(',') as (u:int, v:int);
Graph_reverse = foreach Graph generate v as u, u as v;
Graph = union Graph, Graph_reverse;
Graph = distinct Graph;

Graph_r = foreach Graph generate u as u, v as v;

Graph_adjacent_edges = join Graph by u, Graph_r by u;

Graph_diag = foreach Graph_adjacent_edges generate Graph::v as v1, Graph_r::v as v2, Graph::u as u;

Graph_diag = filter Graph_diag by v1 < v2;

Graph_diag_g = group Graph_diag by (v1, v2);

Graph_diag_f = foreach Graph_diag_g generate flatten(group) as (v1, v2), Graph_diag.u as u;

Graph_quadrilaterals = filter Graph_diag_f by SIZE(u) == 2;

Graph_quadrilaterals = foreach Graph_quadrilaterals generate v1, v2, flatten(u) as u1, flatten(u) as u2;

Graph_quadrilaterals = filter Graph_quadrilaterals by u1 < u2;

Graph_quadrilaterals = foreach Graph_quadrilaterals generate TOMAP((chararray)v1,'',(chararray)v2,'',(chararray)u1,'',(chararray)u2,'');

Graph_quadrilaterals = distinct Graph_quadrilaterals;

dump Graph_quadrilaterals;  
