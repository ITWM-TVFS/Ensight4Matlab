%% setVariable(object, part, name, values, vartype, timestep)
% Set variable values for one part
%
% INPUT
%  object   : |EnsightObj| object
%  part     : (int||string) part_idetifier 
%  values   : (numeric) Dimensions depend in vartype
%  vartype  : (int||string) Either 0,1,2 or 'ConstantPerCase','ScalarPerNode','VectorPerNode'
%  timestep : (integer) Not needed if the object is static
%
% OUTPUT
%  none
%
% USAGE
%  object.setVariable(part, name, values, vartype, timestep)
%  object.setVariable(part_identifier, name, values, vartype, timestep)
%

%%
function setVariable(this, part, name, values, vartype, timestep)
    % Check edit mode
    assert(this.inEditMode(),'EnsightLib::setVariable - Not in edit mode'); 
  
    % verify input
    pid = this.verifyPartIdentifier(part);
    
    assert(this.hasVariable(name),'EnsightLib::setVariable - Variable does not exist.');
    
    if nargin<6
        assert(~this.isTransient(),'For non-static objects, the timestep must be specified');
        timestep = 0;
    end
    
    assert(this.verifyTimestep(timestep),'EnsightLib::setVariable - Invalid timestep');
    
    type_id = this.verifyVariableType(vartype);
    assert(type_id > -1, 'Invalid variable type');
    var_dims = [1 1 3];
    assert(size(values,1) == var_dims(type_id+1), 'EnsightLib::setVariable - values dimensions do not coincide with vartype.');
    
    var_idx = this.getVariableIndex(name);
    switch type_id
        case 0
            this.EnsightVariableList{var_idx,3} = 1;
        case 1
            this.EnsightVariableList{var_idx,3} = 1;
        case 2
            this.EnsightVariableList{var_idx,3} = 3;
    end
    
    valueArray = [6, 0, 5, 0, 1, 0, 0, 0];
    EnsightLib_interface('part', 'setVariable', valueArray, this.getObjectHandle(), pid, name, values, type_id, timestep);
end