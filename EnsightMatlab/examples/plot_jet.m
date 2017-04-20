%% Plot variables of the jet data set using different methods
% 
% This plots variables of the jet example data set. The different plot
% methods used are:
% scatter plot: create a scatter plot of data at cell vertices.
%               Only the vertices are plotted, not the cell geometry.
%               (e.g. for a triangular cell, the variable data at the 3
%                triangle vertices is plotted but not the triangle itself)
% interpolated plot: the data is interpolated over a dense grid

% Temporarily adjust MATLAB path to find EnsightLib wrapper if not in path.
temp_path = path;
path(temp_path,'../');

fileName = '../../data/jet.encas';

if exist(fileName, 'file') ~= 2
    error(['Input file "' inName '" not found. Please make sure you '...
           'have unpacked the example data and run this script from '...
           'the correct diretory!']);
end

% read input file
jet = EnsightLib(fileName);

% scatter plot some variables
close all;
subplot(2,2,1);
title('Temperature (at vertices)');
scatter_plot_all(jet, 'temperature', false);

subplot(2,2,2);
title('Density (at vertices)');
scatter_plot_all(jet, 'density', true);

subplot(2,2,3);
title('Turbulent kinetic energy (at vertices)');
scatter_plot_all(jet, 'turb_kinetic_energy', true);


% Plot interpolated values for temperature over a grid.
% This takes longer because data at every single point of the grid is
% interpolated over an unstructured mesh.
subplot(2,2,4);
nx = 200; ny = 300;
xrange = linspace(-0.5, 0.5, nx);
yrange = linspace(0.1, 1.4, ny);
plot_interpolation(jet, 'temperature', xrange, yrange);
title('Temperature (interpolated)');

% Restore original path
path(temp_path);


function scatter_plot_all(ensObj, variableName, project2d)
    hold on;
    partList = ensObj.EnsightPartList;
    for p = 1:length(partList)
        partIdx = partList{p, 1};
        scatter_plot_part(ensObj, partIdx, variableName, project2d);
    end
    axis equal;
end

function scatter_plot_part(ensObj, part, variableName, project2d)
    vertices = ensObj.getVertices(part);
    % color by variable
    variable = ensObj.getVariable(part, variableName);
    % get vertex coordinates (ignoring cell connectivity)
    x = vertices(1, :);
    y = vertices(2, :);
    if project2d
        % ignore z coordinate and project into x-y-plane
        scatter(x, y, 2, variable);
    else
        z = vertices(3, :);
        scatter3(x, y, z, 2, variable);
    end
end

function plot_interpolation(ensObj, variableName, xrange, yrange)
    v = zeros(size(xrange, 2), size(yrange, 2));
    for xi = 1:size(xrange, 2)
        for yi = 1:size(yrange, 2)
            pt = [xrange(xi) yrange(yi) 0]';
            v(xi, yi) = ensObj.interpolateVariable(pt, variableName);
        end
    end
    imagesc(v');
    set(gca, 'YDir', 'normal')
end

