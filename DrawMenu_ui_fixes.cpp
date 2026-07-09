// =============================================================================
// DrakoX DrawMenu UI fixes
// Apply the sections below into your existing source file.
// =============================================================================

// ─── FIX A: SaveKey — flush so key persists after app close ───────────────────
static void SaveKey(const char* key){
    if(!key||!key[0])return;
    EnsureKeyDir();
    std::ofstream f(GetKeyFilePath(),std::ios::out|std::ios::trunc|std::ios::binary);
    if(!f.is_open())return;
    f<<key;
    f.flush();
    f.close();
}

// ─── FIX B: SidebarButtonModern — no orphan PopFont, better spacing ───────────
// Replace your existing SidebarButtonModern lambda with this:

auto SidebarButtonModern=[&](const char* label,const char* icon,int id,bool active)->bool{
    ImVec2 p=ImGui::GetCursorScreenPos();
    float w=sidebarW-10.0f;
    float h=fixedBtnH;
    ImDrawList* dl=ImGui::GetWindowDrawList();
    bool hovered=ImGui::IsMouseHoveringRect(p,ImVec2(p.x+w,p.y+h));
    bool clicked=hovered&&ImGui::IsMouseClicked(0);

    ImU32 bgCol=active?IM_COL32(90,35,155,180):(hovered?IM_COL32(55,20,100,120):IM_COL32(0,0,0,0));
    dl->AddRectFilled(p,ImVec2(p.x+w,p.y+h),bgCol,8.0f);
    if(active){
        dl->AddRectFilled(ImVec2(p.x,p.y+8.0f),ImVec2(p.x+3.0f,p.y+h-8.0f),IM_COL32(180,110,255,255),2.0f);
    }

    const float iconTop=11.0f;
    const float labelTop=h-21.0f;

    ImVec2 iconSize=ImGui::CalcTextSize(icon);
    ImGui::PushStyleColor(ImGuiCol_Text,active?ImVec4(0.92f,0.74f,1.0f,1.0f):ImVec4(0.52f,0.47f,0.62f,1.0f));
    ImGui::SetCursorScreenPos(ImVec2(p.x+(w-iconSize.x)*0.5f,p.y+iconTop));
    ImGui::TextUnformatted(icon);
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text,active?ImVec4(1.0f,1.0f,1.0f,1.0f):ImVec4(0.58f,0.53f,0.68f,1.0f));
    ImGui::SetWindowFontScale(0.78f);
    ImVec2 labelSz=ImGui::CalcTextSize(label);
    ImGui::SetCursorScreenPos(ImVec2(p.x+(w-labelSz.x)*0.5f,p.y+labelTop));
    ImGui::TextUnformatted(label);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor();

    ImGui::SetCursorScreenPos(p);
    ImGui::Dummy(ImVec2(w,h));
    return clicked;
};

// ─── FIX C: LOAD SAVED KEY — re-read from disk every click ────────────────────
// Replace your LOAD SAVED KEY button block:

if(ImGui::Button(ICON_MD_HISTORY " LOAD SAVED KEY",{-1,32})){
    savedKey=LoadKeyFile();
    if(!savedKey.empty()){
        strncpy(keyBuf,savedKey.c_str(),sizeof(keyBuf)-1);
        keyBuf[sizeof(keyBuf)-1]=0;
        ImGui::InsertNotification({ImGuiToastType_Info,2000,OBFUSCATE(ICON_MD_KEY " Key loaded!")});
    }else{
        ImGui::InsertNotification({ImGuiToastType_Warning,2000,OBFUSCATE(ICON_MD_ERROR " No saved key")});
    }
}

// ─── FIX D: Login verify — save key + clear err on success ────────────────────
// Replace the verifyT>=1.3f block:

if(verifyT>=1.3f){
    err=Login(keyBuf);
    if(err==std::string("OK")&&bValid&&g_Auth==g_Token){
        SaveKey(keyBuf);
        savedKey=std::string(keyBuf);
        err.clear();
        isLogin=true;
        ImGui::InsertNotification({ImGuiToastType_Success,3000,OBFUSCATE(ICON_MD_CHECK " Login Success!")});
    }else{
        ImGui::InsertNotification({ImGuiToastType_Error,3000,(std::string(OBFUSCATE(ICON_MD_CLOSE " Wrong Key: "))+err).c_str()});
    }
    loginPhase=0;
    verifyT=0.f;
}

// ─── FIX E: Language pill — remove BOTH orphan ImGui::PopFont() lines ─────────
// DELETE these two lines from the lang pill block (they crash — no matching PushFont):
//
//   ImGui::PopFont();   // line before float labelW=...
//   ImGui::PopFont();   // line after ImGui::TextUnformatted(ICON_MD_LANGUAGE);
//
// Correct lang pill sizing block should start like:
//
//   float iconW=ImGui::CalcTextSize(ICON_MD_LANGUAGE).x;
//   float labelW=ImGui::CalcTextSize(langNames[lidx]).x;
//   float pillW=iconW+6.0f+labelW+24.0f;
//   ... (no PopFont anywhere in this block)

// ─── FIX F: Content tab header — remove orphan PopFont ────────────────────────
// DELETE this line after icon_tab text:
//
//   ImGui::PopFont();
//
// Should be only:
//   ImGui::Text("%s",icon_tab);
//   ImGui::PopStyleColor();
