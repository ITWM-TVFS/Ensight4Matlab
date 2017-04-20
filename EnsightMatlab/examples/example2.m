% Example 2
% Create Quadrangle cells with alternating normals

% Temporarily adjust MATLAB path to find EnsightLib wrapper if not in path.
temp_path = path;
path(temp_path, '../');

obj = EnsightLib();

nx = 10;
nz = 10;
x = linspace(0, 1, nx);
z = linspace(0, 1, nz);

vert = zeros(3, nx*nz);
for j=1:nz
    for i=1:nx
        vert(:, (j-1)*nx+i) = [x(i); z(j); 0.0];
    end
end

cell = zeros(4, (nx-1)*(nz-1));
for j=1:(nz-1)
    for i=1:(nx-1)
        cell(:, (j-1)*(nx-1)+i) = [(j-1)*nx+i-1; (j-1)*nx+i; j*nx+i; j*nx+i-1];
        if mod(i, 2)==0
            cell(:, (j-1)*(nx-1)+i) = cell([1 4 3 2], (j-1)*(nx-1)+i);
        end
    end
end

obj.beginEdit;
obj.setStatic;
obj.createEnsightPart('quads');
obj.setVertices('quads', vert);
obj.setCells('quads', cell, 'Quadrangle');
obj.endEdit;
obj.writeCase('data/example2/example2.case');

clear obj;

% Restore original path
path(temp_path);