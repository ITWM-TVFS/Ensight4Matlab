% Example 1
% Simple EnSight setup and coordinate interpolation

% Temporarily adjust MATLAB path to find EnsightLib wrapper if not in path.
temp_path = path;
path(temp_path,'../');

% Create object and enter edit mode
obj = EnsightLib();

obj.beginEdit();
obj.setStatic;

% Create single part with one cubic cell
obj.createEnsightPart('part1', 1);
vert = [0 0 0; 1 0 0; 1 1 0; 0 1 0; 0 0 1; 1 0 1; 1 1 1; 0 1 1]';
cell = (0:7)';

obj.setVertices(1, vert);
obj.setCells('part1', cell, 'Hexahedron');

% Create vector-valued variable with position information
obj.createVariable('position', 'VectorPerNode');
obj.setVariable(1, 'position', vert, 'VectorPerNode');

var2 = sum(vert, 1);
obj.createVariable('testVariable', 'ScalarPerNode');
obj.setVariable(1, 'testVariable', var2, 'ScalarPerNode');

obj.endEdit;
% Write object into .case file
obj.writeCase('data/example1/example1.case');

% Will be created automatically. If this is called manually, it needs to be
% called after editing the object is completed.
%obj.createSubdivTree(5, 5, 0);

% Test interpolation for random points
test_points = rand(3, 10);
interp_points = zeros(1, 10);
varName = 'testVariable';
for i=1:10
    point = test_points(:, i);
    [interp_points(i), ok] = obj.interpolateVariable(point, varName);
end

interp_points
cell_index = obj.search(test_points,'vc');


clear obj;

% Restore original path
path(temp_path);