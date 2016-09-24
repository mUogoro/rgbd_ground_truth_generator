%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% test.m
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% SETUP PARAMETERS

% Select the model.
MODEL = 'hand';
%MODEL = 'human';

% Rendering parameters
% - Resolution
RENDER_WIDTH=640;
RENDER_HEIGHT=480;
% - Horizontal and vertical field of view (in degrees)
RENDER_HFOV=57;
RENDER_VFOV=43;
% - Near and Far clipping plane distances (in cm)
RENDER_NEAR=20;
RENDER_FAR=1000;

% Distance of the model from the camera
if strcmp(MODEL, 'hand')
    MODEL_DIST = 50;
else
    MODEL_DIST = 400;
end

% Path of the model file to be load
% NOTE: the model file (typically a Collada (.dae) file) must contain
% at least two objects:
% - a mesh object named "HandMesh"
% - a skeleton object named "Mesh"
MODEL_PATH=['../../test_models/' MODEL '.dae'];


%% INITIALIZATION

% Get a model renderer instance
mrendInst = model_renderer_new(RENDER_WIDTH, RENDER_HEIGHT, ...
                               MODEL_PATH, RENDER_HFOV, RENDER_VFOV, ...
                               RENDER_NEAR, RENDER_FAR);
disp('Hand loaded')

% Get the number of bones
nBones = model_renderer_getbonesnum(mrendInst);

% The pose matrix is a 4x4xnBones matrix, i.e. a vector of 4x4 matrices,
% where each element represents the homegeneous transform applied to the
% skeleton bone with that index.  By convention, the first matrix always
% represents the gloabl rigid transformation applied to the model.
% Initialize the pose matrix assigning the identity matrix to each element
pose = zeros(4,4,nBones);
for i=1:nBones, pose(:,:,i) = eye(4); end

% Camera placement: by design the camera is always at the world origin.
% Axis are oriented using the OpenGL convention, i.e. we use a right
% handed coordinate system and the camera is pointing towards the 
% negative Z axis, whereas the camera up vector correspond to the global
% Y axis.
% Place the model a -MODEL_DIST cm from the camera along the -Z axis
pose(3,4,1) = -MODEL_DIST;


% Animating the model: the model is animated by setting the homogeneous
% matrix (typically a rotation matrix) of a specific bone. Given the
% name of the bone, we must first retrieve its index in the pose matrix
if strcmp(MODEL, 'human')
    boneNames = {'upperarm_l', 'upperarm_r'};
else
    boneNames = {'ring_1_R', 'ring_2_R', 'ring_3_R', ...
                 'middle_1_R', 'middle_2_R', 'middle_3_R'};
end

for i=1:length(boneNames)
    % Note: the bone index follows the C indexing conventin, i.e. indices
    % start from 0. Increment by 1 to account for different matlab indexing
    boneID = model_renderer_getboneidx(mrendInst, boneNames{i}) + 1;
    
    % Given the bone ID, we can set the corresponding matrix. For example,
    % rotate each bone in the boneNames vector by 90 degrees along the
    % X axis
    rotMat = [1 0 0 0;
              0 cos(pi/2) -sin(pi/2) 0;
              0 sin(pi/2) cos(pi/2) 0;
              0 0 0 1];
    pose(:,:,boneID) = rotMat;
end

% Finally, animate the model by setting the pose
model_renderer_setpose(mrendInst, pose);
disp('Hand pose set')

%% Rendering

% Simply call the model_renderer_render method to perform the rendering.
% The function returns two images: a RGB rendering and a 16 bit depthmap
[colour, depth] = model_renderer_render(mrendInst);
imshow(colour);

%% Cleanup

% Always free the memory allocated for the rendering
model_renderer_delete(mrendInst);
