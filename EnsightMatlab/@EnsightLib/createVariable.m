%% createVariable(object, name, type)
% INPUT
%  object : |EnsightLib| object
%  name   : variable name (string)
%  type   : (int || string) Either {0,1,2} or {'ConstantPerCase','ScalarPerNode','VectorPerNode'}
%
% OUTPUT
%  none
%
% USAGE
%  object.createVariable(name, type)
%

%%
function createVariable(this, name, vartype)
    
    % Check input
    assert(isa(this,'EnsightLib'),'EnsightLib::createVariable - Input1(object) is not valid. Type `help EnsighLib.createVariable` for detailed information');
    assert(isa(name,'char'),'EnsightLib::createVariable - Input2(name) is not valid. Type `help EnsighLib.createVariable` for detailed information');
    assert(isnumeric(vartype) || isa(vartype,'char'),'EnsightLib::createVariable - Input3(type) is not valid. Type `help EnsighLib.createVariable` for detailed information');
    assert(this.inEditMode(),'EnsightLib::createVariable - Not in edit mode');
     
    % Check uniqueness of identifiers
    assert(~any(ismember(this.EnsightVariableList(:,1),name)),['EnsightLib::createVariable - An EnsightVariable with name `',name,'` already exists.']);
      
    % Get variable type
    type_id = this.verifyVariableType(vartype);
    assert(type_id>-1,'EnsightLib::createVariable - Invalid variable type');
  
    valueArray = [3, 0, 5, 1, 0];
    EnsightLib_interface('obj', 'createVariable', valueArray, this.getObjectHandle(), name, type_id);
  
    n = size(this.EnsightVariableList,1) + 1; % Index of new variable
    this.EnsightVariableList{n,1} = name; 
    typelist = {'ConstantPerCase','ScalarPerNode','VectorPerNode'};
    this.EnsightVariableList{n,2} = typelist{type_id+1};
    this.EnsightVariableList{n,3} = 0; % empty variable. Assign dimensions 0x0
end