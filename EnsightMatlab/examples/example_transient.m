% Example for transient data
% Create triangles with changing variable values

% Temporarily adjust MATLAB path to find EnsightLib wrapper if not in path.
temp_path = path;
path(temp_path, '../');

% Create object and enter edit mode
obj = EnsightLib();
obj.beginEdit();

% set the time steps
timeVector = [0, 0.5, 1, 1.5];
obj.setTransient(timeVector);

% Create single part with two triangle cells
partId = 1;
obj.createEnsightPart('part1', partId);

% Create a variable
obj.createVariable('values', 'ScalarPerNode');

% data for 4 time steps
for timeStep = 0:3
    ts = timeStep;
    % vertices of the triangles
    vert = [0 0 0;    1 0 0; 0 1 0; ...
            1 0 ts/3; 1 1 1; 0 1 ts]';
    % Cell indices of the triangles, i.e. each triangle consists of 3
    % indices into the vertex array (zero-based indices)
    cell = [0:2; 3:5]';
    % variable values
    vals = [1 ts ts*ts/4 sin(ts) cos(ts) 1];

    obj.setVertices(partId, vert, timeStep);
    obj.setCells(partId, cell, 'Triangle', timeStep);
    obj.setVariable(partId, 'values', vals, 'ScalarPerNode', timeStep);
end

obj.endEdit;

% write data for all time steps
obj.writeCase('data/example_transient/example_transient.case', -1);

% Alternatively, the individual time steps can be written for each time
% step while they are being computed (useful for long running computations)
%
%for timeStep = 0:3
%    obj.writeCase('data/example_transient/example_transient.case', timeStep);
%end

clear obj;

% Restore original path
path(temp_path);