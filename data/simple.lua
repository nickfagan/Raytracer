-- A simple scene with five spheres

mat1 = gr.material({0.7, 0.0, 0.0}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.0, 0.0, 0.7}, {0.5, 0.7, 0.5}, 25)

scene_root = gr.node('root')


--no heir
--s1 = gr.nh_sphere('s1', {2.5, 0, -10}, 5)
--scene_root:add_child(s1)
--s1:set_material(mat1)

--s2 = gr.nh_sphere('s2', {-2.5, 0, 0}, 5)
--scene_root:add_child(s2)
--s2:set_material(mat2)


s1 = gr.sphere('s1')
s1:translate(2.5, 0, -10)
s1:scale(5, 10, 5)
s1:set_material(mat1)
scene_root:add_child(s1)

s2 = gr.sphere('s2', {-2.5, 0, 0}, 5)
s2:translate(-2.5, 0, 0)
s2:scale(5, 5, 5)
s2:set_material(mat2)
scene_root:add_child(s2)



white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0}, 1.0)

gr.render(scene_root, 'simple.png', 256, 256,
	  {0, 0, 50}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
