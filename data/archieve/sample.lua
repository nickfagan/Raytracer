-- A simple scene with five spheres

mat1 = gr.ref_material({0.5, 0.5, 0.5}, {0.7, 0.7, 0.7}, 10, 0.5)
boxMat = gr.material({0.2, 0.2, 0.2}, {0.3, 0.3, 0.3}, 5)
mat3 = gr.ref_material({0.8, 0.8, 0.0}, {0.5, 0.5, 0.5}, 25, 0.5)

scene_root = gr.node('root')


b1 = gr.nh_box('b1', {-200, -200, -400}, 400)
scene_root:add_child(b1)
b1:set_material(boxMat)

s1 = gr.nh_sphere('s1', {0, 0, 0}, 75)
scene_root:add_child(s1)
s1:set_ref_material(mat3)

s2 = gr.nh_sphere('s2', {-120, 0, 25}, 35)
scene_root:add_child(s2)
s2:set_ref_material(mat1)

s3 = gr.nh_sphere('s3', {120, 0, 25}, 35)
scene_root:add_child(s3)
s3:set_ref_material(mat1)


white_light = gr.light({-400, 400, 400}, {0.9, 0.9, 0.9}, {1, 0, 0})
red_light = gr.box_light({0.0, 100.0, 100.0}, {0.0, 1.0, 1.0}, {1, 0, 0}, 10)

gr.render(scene_root, 'sample.png', 1024, 512,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, red_light})



