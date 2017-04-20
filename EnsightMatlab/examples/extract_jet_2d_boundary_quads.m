%% Extract 2D slice from example 3D data set jet.encas
% This example script reads the 3D data set jet.encas and extracts a 2D
% slice of the data, saving it as jet_2d.encas.
%
% The input data set represents a z-periodic 3D section of a flow
% simulation. Besides the volumetric cells, it also contains 2D data at the
% periodic domain boundaries (z=+0.05 and z=-0.05). Therefore, this script
% only processes the parts containing the boundaries and extracts one of
% the two boundaries by discarding all cells/vertices with negative
% z-coordinate.

% Temporarily adjust MATLAB path to find EnsightLib wrapper if not in path.
temp_path = path;
path(temp_path, '../');

inName = '../../data/jet.encas';
outName = 'data/jet_2d.encas';

if exist(inName, 'file') ~= 2
    error(['Input file "' inName '" not found. Please make sure you '...
           'have unpacked the example data and run this script from '...
           'the correct diretory!']);
end

% read input file
ensIn = EnsightLib(inName);

% prepare output object
ensOut = EnsightLib();
ensOut.beginEdit();
ensOut.setStatic();

% create the same set of variables
for vi = 1:length(ensIn.EnsightVariableList)
    vname = ensIn.EnsightVariableList{vi, 1};
    vtype = ensIn.EnsightVariableList{vi, 2};
    ensOut.createVariable(vname, vtype);
end

% Loop over the boundary parts containg quadrangles
for pn = {'periodic_1a', 'periodic_2a', 'periodic_3a'}
    pname = pn{1};
    vertices = ensIn.getVertices(pname);
    % This assumes that the vertices lie in two z-parallel planes and that
    % all vertices with z-coord > 0 come first.
    % get the vertices with z > 0
    vertices2 = vertices(:, vertices(3,:) > 0);
    numVerts = size(vertices2, 2);
    % Check that our assumptions regarding the data set are correct.
    assert(min(vertices(3, 1:numVerts)) >= 0);
    assert(max(vertices2(3, :)) - min(vertices2(3, :)) <= 1e-10);

    % get the corresponding cells
    cells = ensIn.getCellValues(pname, 'Quadrangle');
    cells2 = cells(:, max(cells) < numVerts);

    % write vertices and cells
    ensOut.createEnsightPart(pname);
    ensOut.setVertices(pname, vertices2);
    ensOut.setCells(pname, cells2,  'Quadrangle');

    % write the corresponding variables
    for vi = 1:length(ensIn.EnsightVariableList)
        vname = ensIn.EnsightVariableList{vi, 1};
        vtype = ensIn.EnsightVariableList{vi, 2};
        values = ensIn.getVariable(pname, vname);
        assert(size(values, 2) == size(vertices, 2));
        values2 = values(:, 1:numVerts);
        ensOut.setVariable(pname, vname, values2, vtype);
    end
    
end

ensOut.endEdit();
ensOut.writeCase(outName );

% Restore original path
path(temp_path);
