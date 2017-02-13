%% EnsightLib
% MATLAB interface for the EnsightLib Cpp-library
%
% Type 'methods EnsightLib' or 'help EnsightLib.method' for more information
%

% Copyright (c) 2016 Fraunhofer ITWM
%
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, including without limitation the rights
% to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
% copies of the Software, and to permit persons to whom the Software is
% furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included in
% all copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
% OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
% SOFTWARE.
%

classdef EnsightLib < handle
    %% EnsightLib
    % MATLAB interface for the EnsightLib Cpp-library
    %
    % Type 'methods EnsightLib' or 'help EnsightLib.method' for more information
    %

    %% 
    
    
    % Memory Structure: Hidden properties
    properties (SetAccess = private, Hidden = true)
        EnsightObjectHandle; 	% Handle to the underlying C++ class instance
    end

    % Object Structure: Visible but private access
    properties (SetAccess = private)
        timeSteps;              % (int) Nx1 vector containing timesteps
        editMode;               % (bool) edit mode enabled/disabled
        EnsightPartList;        % Cell Array {Id, 'partName'}
        EnsightSubdivTree;      % struct{(bool) exists, maxlevel, maxelements, offset, octant, bounds}
        EnsightVariableList;    % Cell Array {'name', type, dimensions}
        Constants;              % Cell Array {'constantName', value}
    end
    
    %----------------------------------------------------------------------------------------------------------
    % ####################################### Internal (hidden) methods #######################################
    %----------------------------------------------------------------------------------------------------------
    methods (Hidden = true)
      
      % Return handle-pointer to cpp-instance of this class
      function ptr = getObjectHandle(this)
          ptr = this.EnsightObjectHandle;
      end
      
      % Verify correctness of part_name / part_id
      % Return part_id if correct, -1 otherwise
      function id = verifyPartIdentifier(this, identifier)
        if(isnumeric(identifier) && floor(identifier)==identifier)
            [ok,idx] = ismember(identifier,cell2mat(this.EnsightPartList(:,1)));
        elseif isa(identifier,'char')
            [ok,idx] = ismember(identifier,this.EnsightPartList(:,2));
        end
        
        if ~ok
            id = -1;
        else
            id = this.EnsightPartList{idx,1};
        end
      end
      
      % Get the index of an EnsightPart inside EnsightPartList
      function idx = getPartIndex(this, identifier)
        if(isnumeric(identifier) && floor(identifier)==identifier)
            [ok,idx] = ismember(identifier,cell2mat(this.EnsightPartList(:,1)));
        elseif isa(identifier,'char')
            [ok,idx] = ismember(identifier,this.EnsightPartList(:,2));
        end
        
        if ~ok
            idx = -1;
        end
      end
      
      % Verify correctness of cellType / cellType_id
      % Return cellType_id if correct, -1 otherwise
      function ident = verifyCellType(this, celltype)
          if isnumeric(celltype) && floor(celltype)==celltype
            if 0<=celltype && celltype<=7
                ident = celltype;
            else
                ident = -1;
            end
          elseif isa(celltype,'char')
              typelist = {'Point','Bar','Triangle','Quadrangle','Tetrahedron','Pyramid','Wedge','Hexahedron'};
              [ok,ident] = ismember(celltype,typelist);
              ident = ident-1;
              if ~ok
                ident = -1;
              end
          else
              ident = -1; 
          end
      end
      
      % Get number of vertice per celltype
      function num = getNumberOfNodes(this, celltype)
          numV = [1 2 3 4 4 5 6 8];
          cid = this.verifyCellType(celltype);
          num = numV(cid+1);
      end
      
      % Verify correctness of variableType / variableType_id
      % Return variableType_id if correct, -1 otherwise
      function type_id = verifyVariableType(this, vartype) 
          if isnumeric(vartype) && floor(vartype)==vartype
              if 0<=vartype && vartype<=2
                  type_id = vartype;
              else
                  type_id = -1;  
              end
          elseif isa(vartype,'char')
              typelist = {'ConstantPerCase','ScalarPerNode','VectorPerNode'};
              [ok,type_id] = ismember(vartype,typelist);
              type_id = type_id -1;
              if ~ok
                  type_id = -1;
              end    
          else
              type_id = -1;
          end
      end
      
      % Get the index of an EnsightVariable inside EnsightVariableList
      function idx = getVariableIndex(this, name)
        if isa(name,'char')
            [ok,idx] = ismember(name,this.EnsightVariableList(:,1));
        else
            error('EnsightLib::getVariableIndex() - VariableName has wrong data type. String expected.');
        end
        
        if ~ok
            idx = -1;
        end
      end
      
      % Verify timestep_id
      function ok = verifyTimestep(this, timesteps) 
          if this.isTransient
            ok = all(timesteps>=0) && all(floor(timesteps)==timesteps) && max(timesteps)<this.getNumberOfTimesteps();
          else
              ok = timesteps==0 && length(timesteps)==1;
          end
      end
  end
  
  methods
    
    %-------------------------------------------------------------------------------------------------------------
    % ######################################### Constructor/Destructor //#########################################
    
    % // Standard Constructor // %%
    function this = EnsightLib(filename)
        assert(usejava('Desktop') == 0 || ...
               verLessThan('matlab', '8.5') || ~verLessThan('matlab', '9.0'), ...
               ['The current release of EnsightMatlab shows stabilty problems with the 2015 desktop version of Matlab. ' ...
                'Run "matlab -nojvm" or "matlab -nodesktop" in order to use it, or remove this assertion to ignore the warning.']);
        % Empty object
        if nargin == 0
            valueArray = [0, 1];
            this.EnsightObjectHandle = EnsightLib_interface('interface', 'create', valueArray);
            this.EnsightPartList = cell(0,2);
            this.EnsightSubdivTree = struct('created',false,'maxLevel',0,'maxElements',0,'offset',0.0,'bounds',[0 0; 0 0; 0 0]);
            this.EnsightVariableList = cell(0,3);
            this.Constants = cell(0,2);
            this.timeSteps = 0;
            this.editMode = false;
        
        % Create from file
        elseif nargin == 1
            valueArray = [1, 6, 1];
            [this.EnsightObjectHandle,...
             this.EnsightPartList,...
             this.Constants,...
             this.EnsightVariableList,...
             this.EnsightSubdivTree,...
             this.timeSteps] = EnsightLib_interface('interface', 'read', valueArray, filename);
            this.editMode = false;
            typelist = {'ConstantPerCase','ScalarPerNode','VectorPerNode'};
            types = cell2mat(this.EnsightVariableList(:,2));
            if(~isempty(types))
                this.EnsightVariableList(:,2) = typelist(types+1);
            end
        else
            error('EnsightObj(): Wrong number of input parameters.');
        end
    end

    % // Standard Destructor // %%
    % Delete this MATLAB-object and call destructors of all contained
    % objects
    function delete(this)
        % Delete object data
        this.EnsightPartList = [];
        this.EnsightSubdivTree = [];
        this.EnsightVariableList = [];
        this.Constants = [];
        this.timeSteps = [];
        this.editMode = [];
      
      % Call C++-destructor
      EnsightLib_interface('interface', 'delete', [1, 0, 5], this.getObjectHandle());
      this.EnsightObjectHandle = [];
    end
    
    %----------------------------------------------------------------------------------------------------------
    % ######################################## EnsightObj methods #############################################
    %----------------------------------------------------------------------------------------------------------
    
    % Write data
    writeCase(varargin);
    
    % Enable edit mode
    beginEdit(this);
    
    % Leave edit mode and verify all changes
    % Error handling is done by the underlying C++ class
    endEdit(this);
    
    % return: (bool) edit mode enabled
    ok = inEditMode(this);
    
    % create new |EnsightPart|
    % return [part_id, part_name]
    varargout = createEnsightPart(this, name, id);

    % Return number of |EnsightParts| stored in this object
    num = getNumberOfParts(this);

    % Get maximal PartID in use
    id = getMaxId(this);

    % Set timestep vector
    setTransient(this, values);

    % Set timestep vector to 0
    setStatic(this);

    % (true) if #timesteps > 1
    ok = isTransient(this);

    % returns length of .timesteps.
    num = getNumberOfTimesteps(this);

    % returns (vector) .timesteps.
    timesteps = getTimesteps(this);

    % Erase data at a given timestep
    clean(this, timestep);

    % Set vertices of a specific part
    setVertices(this, part, vertices, timestep);

    % Set cells contained in a specific part at 'timestep'
    setCells(this, part, cells, celltype, timestep);

    % Create a new variable
    createVariable(this, name, vartype);

    % returns #variables
    num = getNumberOfVariables(this);

    % 
    varargout = getVariable(this, varargin);
    
    % returns 'true' if variable with given name exists, 'false' else
    % optional: returns 'id' 
    varargout = hasVariable(this, name);

    % Select variable 'name' at 'timestep' and set it to 'values'
    setVariable(this, part, name, values, vartype, timestep);

    % Create a new constant 'name' and insert 'value'
    addConstant(this, name, value);

    % returns 'true' if constant with given name exists, 'false' else
    % optional: returns 'id' 
    varargout = hasConstant(this, name);

    % Returns value of the constant 'name'
    value = getConstant(this, name);

    % Return cell {'name', value} of all constants
    constants = getConstants(this);

    % Replace ALL constants with a new cell array {'name',value} of constants
    replaceConstants(this, varargin);

    % Return #constants
    num = getNumberOfConstants(this);

    % Return boundary matrix of variable 'name' at 'timestep'
    varargout = getVariableBounds(this, name, timestep);

    % Create |EnsightSubdivTree| for this object and save class handle
    createSubdivTree(this, maxDepth, maxElements, sizeOffset);

    % Print all information about this |EnsightObject|
    print(this);
    
    
    %----------------------------------------------------------------------------------------------------------
    % ########################################## EnsightPart methods ##########################################
    %----------------------------------------------------------------------------------------------------------
    
   
    % (char) Return this part's name
    name = getPartName(this,part);

    % Return (double)(3x2) axis aligned bounding box for the geometry of this part
    Bbox = getPartBounds(this, part, timestep);

    % Get vertices at one timestep
    varargout = getVertices(varargin);

    % (bool) Cells of type 'celltype' exist at time 'timestep'
    ok = hasCellType(this, celltype, part, timestep);

    values = getCellValues(this, partID, celltypeID, timestep);

    % Get an overview of all cells at time 'timestep'
    % returns: {type, count}
    CList = getCellList(varargin);

    % Get bounding box of one cell(nodes) at time 'timestep'
    Bbox = getCellBounds(this, partID, cell, timestep);
    
    %----------------------------------------------------------------------------------------------------------
    % ######################################### Interpolation methods #########################################
    %----------------------------------------------------------------------------------------------------------
    [result,ok] = interpolateVariable(this, position, variableName);

    cellList = search(this, lineSegments, options);
    
    %cells = getCells(this);
  end
    
end



